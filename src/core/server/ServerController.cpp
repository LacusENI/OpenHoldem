#include "ServerController.h"

#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "GameModel.h"
#include "GameTypes.hpp"

namespace poker {
void ServerController::subscribeEvent() {
    event_bus.subscribe<Command::JoinRequest>(&ServerController::handle, this);
    event_bus.subscribe<Command::StartGame>(&ServerController::handle, this);
    event_bus.subscribe<Command::SubmitAction>(&ServerController::handle, this);
}

void ServerController::handle(const Command::StartGame& _) {
    std::optional<Event::GameStarted> started = game_model.startGame();
    if (!started) return;
    client_gateway.broadcast(*started);
    enterStage(Stage::PREFLOP);
}

void ServerController::startNextHand() {
    std::optional<Event::GameStarted> started = game_model.startNextHand();
    if (!started) return;
    client_gateway.broadcast(*started);
    enterStage(Stage::PREFLOP);
}

void ServerController::handle(const Command::JoinRequest& command) {
    Event::PlayerJoined joined = game_model.joinNewPlayer();
    joined.uid = command.uid;
    client_gateway.broadcast(joined);
    // 新玩家需要知道自己的座位；JoinResponse 只发给本人
    client_gateway.sendTo(joined.seat,
                          Event::JoinResponse{.uid = command.uid, .seat = joined.seat});
}

void ServerController::handle(const Command::SubmitAction& event) {
    std::optional<Event::PlayerActed> acted = game_model.handlePlayerAction(event);
    if (!acted) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        if (auto waiting = game_model.awaitPlayerAction())
            client_gateway.sendTo(waiting->player, *waiting);
        return;
    }
    client_gateway.broadcast(*acted);

    if (!game_model.isBettingRoundEnded()) {
        game_model.moveActingPlayer();
        if (auto waiting = game_model.awaitPlayerAction())
            client_gateway.sendTo(waiting->player, *waiting);
        return;
    }

    finishBettingRound(); // 退还未跟注筹码 + 重置下注轮

    switch (decideAfterBettingRound()) {
    case AfterRound::HandOver:
        client_gateway.broadcast(game_model.finishGame()); // 冗余防御：无人在局
        break;
    case AfterRound::UncontestedSettle: enterUncontestedSettle(); break;
    case AfterRound::RunoutShowdown: enterStage(Stage::SHOWDOWN); break;
    case AfterRound::NextStreet: enterStage(next_stage(game_model.getCurrentStage())); break;
    }
}

ServerController::AfterRound ServerController::decideAfterBettingRound() const {
    if (game_model.isNoOnePlayerAlive()) return AfterRound::HandOver;            // 防御
    if (game_model.isOnlyOnePlayerAlive()) return AfterRound::UncontestedSettle; // 无人竞争
    if (game_model.isStreetsFinished()) return AfterRound::RunoutShowdown; // 已无可下注者
    if (game_model.getCurrentStage() == Stage::RIVER) return AfterRound::RunoutShowdown;
    return AfterRound::NextStreet;
}

void ServerController::enterStage(Stage stage) {
    client_gateway.broadcast(game_model.changeStage(stage)); // 发布 StageChanged
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
        client_gateway.sendTo(event.player_seat, event); // 私密：单传
    game_model.startBettingRound();
    client_gateway.broadcast(game_model.postSmallBlind());
    client_gateway.broadcast(game_model.postBigBlind());
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_gateway.sendTo(waiting->player, *waiting); // 只通知当前行动者
}

void ServerController::enterPostflop() {
    client_gateway.broadcast(game_model.dealCommunityCards());
    game_model.startBettingRound();
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_gateway.sendTo(waiting->player, *waiting);
}

void ServerController::enterShowdown() {
    client_gateway.broadcast(game_model.dealCommunityCards()); // 补发剩余公共牌
    for (const auto& event : game_model.revealHoleCards())
        client_gateway.broadcast(event); // 摊牌：公开
    enterSettlement();
}

void ServerController::enterSettlement() {
    client_gateway.broadcast(game_model.changeStage(Stage::SETTLEMENT));
    game_model.splitPot();
    for (const auto& event : game_model.distributePots())
        client_gateway.broadcast(event);
    client_gateway.broadcast(game_model.finishGame());
}

void ServerController::enterUncontestedSettle() {
    client_gateway.broadcast(game_model.changeStage(Stage::SETTLEMENT));
    client_gateway.broadcast(game_model.settleUncontested());
    client_gateway.broadcast(game_model.finishGame());
}

void ServerController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    if (auto uncalled = game_model.returnUncalledBet())
        client_gateway.broadcast(*uncalled);
    game_model.closeBettingRound();
}
} // namespace poker
