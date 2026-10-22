#include "ServerController.h"

#include <optional>

#include "Event.hpp"
#include "GameModel.h"
#include "GameTypes.hpp"
#include "IClient.h"

namespace poker {
void ServerController::onStart(const Command::StartGame& _) {
    std::optional<Event::GameStarted> started = game_model.startGame();
    if (!started) return;
    client_hub.broadcast(&IClient::onGameStarted, *started);
    enterStage(Stage::PREFLOP);
}

void ServerController::startNextHand() {
    std::optional<Event::GameStarted> started = game_model.startNextHand();
    if (!started) return;
    client_hub.broadcast(&IClient::onGameStarted, *started);
    enterStage(Stage::PREFLOP);
}

void ServerController::onJoin(IClient& client, const Command::JoinRequest& command) {
    Event::PlayerJoined joined = game_model.joinNewPlayer();
    joined.uid = command.uid;

    // 先登记，再发送：JoinResponse 需要能按座位找到这个 endpoint。
    client_hub.add(joined.seat, client);
    client_hub.broadcast(&IClient::onPlayerJoined, joined);
    client_hub.sendTo(joined.seat, &IClient::onJoinResponse,
                      Event::JoinResponse{.uid = command.uid, .seat = joined.seat});
}

void ServerController::onSubmit(const Command::SubmitAction& command) {
    std::optional<Event::PlayerActed> acted = game_model.handlePlayerAction(command);
    if (!acted) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        if (auto waiting = game_model.awaitPlayerAction())
            client_hub.sendTo(waiting->player, &IClient::onTurnStarted, *waiting);
        return;
    }
    client_hub.broadcast(&IClient::onPlayerActed, *acted);

    if (!game_model.isBettingRoundEnded()) {
        game_model.moveActingPlayer();
        if (auto waiting = game_model.awaitPlayerAction())
            client_hub.sendTo(waiting->player, &IClient::onTurnStarted, *waiting);
        return;
    }

    finishBettingRound(); // 退还未跟注筹码 + 重置下注轮

    switch (decideAfterBettingRound()) {
    case AfterRound::HandOver:
        client_hub.broadcast(&IClient::onGameEnded,
                             game_model.finishGame()); // 冗余防御：无人在局
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
    client_hub.broadcast(&IClient::onStageChanged, game_model.changeStage(stage));
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
        client_hub.sendTo(event.player_seat, &IClient::onHoleCardsDealt, event); // 私密：单传
    game_model.startBettingRound();
    client_hub.broadcast(&IClient::onBlindPosted, game_model.postSmallBlind());
    client_hub.broadcast(&IClient::onBlindPosted, game_model.postBigBlind());
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_hub.sendTo(waiting->player, &IClient::onTurnStarted, *waiting);
}

void ServerController::enterPostflop() {
    client_hub.broadcast(&IClient::onCommunityCardsDealt, game_model.dealCommunityCards());
    game_model.startBettingRound();
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction())
        client_hub.sendTo(waiting->player, &IClient::onTurnStarted, *waiting);
}

void ServerController::enterShowdown() {
    client_hub.broadcast(&IClient::onCommunityCardsDealt,
                         game_model.dealCommunityCards()); // 补发剩余公共牌
    for (const auto& event : game_model.revealHoleCards())
        client_hub.broadcast(&IClient::onHoleCardsRevealed, event); // 摊牌：公开
    enterSettlement();
}

void ServerController::enterSettlement() {
    client_hub.broadcast(&IClient::onStageChanged,
                         game_model.changeStage(Stage::SETTLEMENT));
    game_model.splitPot();
    for (const auto& event : game_model.distributePots())
        client_hub.broadcast(&IClient::onPotAwarded, event);
    client_hub.broadcast(&IClient::onGameEnded, game_model.finishGame());
}

void ServerController::enterUncontestedSettle() {
    client_hub.broadcast(&IClient::onStageChanged,
                         game_model.changeStage(Stage::SETTLEMENT));
    client_hub.broadcast(&IClient::onPotAwarded, game_model.settleUncontested());
    client_hub.broadcast(&IClient::onGameEnded, game_model.finishGame());
}

void ServerController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    if (auto uncalled = game_model.returnUncalledBet())
        client_hub.broadcast(&IClient::onUncalledBetReturned, *uncalled);
    game_model.closeBettingRound();
}
} // namespace poker
