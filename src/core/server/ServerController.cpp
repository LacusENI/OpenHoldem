#include "ServerController.h"

#include <optional>
#include <variant>

#include "Event.hpp"
#include "GameModel.h"
#include "GameTypes.hpp"
#include "ServerHandler.h"

namespace poker {
void ServerController::on(const GameCommand& command, const ClientSession& sender) {
    std::visit([this, &sender](const auto& c) { handle(c, sender); }, command);
}

void ServerController::handle(const Command::JoinRequest& command, const ClientSession& sender) {
    Event::PlayerJoined joined = game_model.joinNewPlayer();
    joined.uid = command.uid;

    // 先登记，再发送：JoinResponse 需要能按座位找到这个 endpoint。
    client_hub.add(joined.seat, sender.handler);
    client_hub.broadcast(joined);
    client_hub.sendTo(joined.seat,
                      Event::JoinResponse{.uid = command.uid, .seat = joined.seat});
}

void ServerController::handle(const Command::StartGame&, const ClientSession&) {
    std::optional<Event::GameStarted> started = game_model.startGame();
    if (!started) return;
    client_hub.broadcast(*started);
    enterStage(Stage::PREFLOP);
}

void ServerController::handle(const Command::SubmitAction& command, const ClientSession& sender) {
    // TODO(P0): 校验 sender.id 对应的座位 == 当前行动者（需要 ClientId → seat 映射）。
    (void)sender;

    std::optional<Event::PlayerActed> acted = game_model.handlePlayerAction(command);
    if (!acted) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        if (auto waiting = game_model.awaitPlayerAction())
            client_hub.sendTo(waiting->player, *waiting);
        return;
    }
    client_hub.broadcast(*acted);

    if (!game_model.isBettingRoundEnded()) {
        game_model.moveActingPlayer();
        if (auto waiting = game_model.awaitPlayerAction())
            client_hub.sendTo(waiting->player, *waiting);
        return;
    }

    finishBettingRound(); // 退还未跟注筹码 + 重置下注轮

    switch (decideAfterBettingRound()) {
    case AfterRound::HandOver:
        client_hub.broadcast(game_model.finishGame()); // 冗余防御：无人在局
        break;
    case AfterRound::UncontestedSettle: enterUncontestedSettle(); break;
    case AfterRound::RunoutShowdown: enterStage(Stage::SHOWDOWN); break;
    case AfterRound::NextStreet: enterStage(next_stage(game_model.getCurrentStage())); break;
    }
}

void ServerController::startNextHand() {
    std::optional<Event::GameStarted> started = game_model.startNextHand();
    if (!started) return;
    client_hub.broadcast(*started);
    enterStage(Stage::PREFLOP);
}

ServerController::AfterRound ServerController::decideAfterBettingRound() const {
    if (game_model.isNoOnePlayerAlive()) return AfterRound::HandOver;            // 防御
    if (game_model.isOnlyOnePlayerAlive()) return AfterRound::UncontestedSettle; // 无人竞争
    if (game_model.isStreetsFinished()) return AfterRound::RunoutShowdown; // 已无可下注者
    if (game_model.getCurrentStage() == Stage::RIVER) return AfterRound::RunoutShowdown;
    return AfterRound::NextStreet;
}

void ServerController::enterStage(Stage stage) {
    client_hub.broadcast(game_model.changeStage(stage));
    if (isPreflop(stage)) {
        enterPreflop();
    } else if (isPostflop(stage)) {
        enterPostflop();
    } else if (stage == Stage::SHOWDOWN) {
        enterShowdown();
    }
    // SETTLEMENT 由各自的结算方法负责
}

void ServerController::enterPreflop() {
    game_model.shuffleDeck();
    for (const auto& event : game_model.dealHoleCards())
        client_hub.sendTo(event.player_seat, event); // 私密：单传
    game_model.startBettingRound();
    client_hub.broadcast(game_model.postSmallBlind());
    client_hub.broadcast(game_model.postBigBlind());
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_hub.sendTo(waiting->player, *waiting);
}

void ServerController::enterPostflop() {
    client_hub.broadcast(game_model.dealCommunityCards());
    game_model.startBettingRound();
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_hub.sendTo(waiting->player, *waiting);
}

void ServerController::enterShowdown() {
    client_hub.broadcast(game_model.dealCommunityCards()); // 补发剩余公共牌
    for (const auto& event : game_model.revealHoleCards())
        client_hub.broadcast(event); // 摊牌：公开
    enterSettlement();
}

void ServerController::enterSettlement() {
    client_hub.broadcast(game_model.changeStage(Stage::SETTLEMENT));
    game_model.splitPot();
    for (const auto& event : game_model.distributePots())
        client_hub.broadcast(event);
    client_hub.broadcast(game_model.finishGame());
}

void ServerController::enterUncontestedSettle() {
    client_hub.broadcast(game_model.changeStage(Stage::SETTLEMENT));
    client_hub.broadcast(game_model.settleUncontested());
    client_hub.broadcast(game_model.finishGame());
}

void ServerController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    if (auto uncalled = game_model.returnUncalledBet())
        client_hub.broadcast(*uncalled);
    game_model.closeBettingRound();
}
} // namespace poker
