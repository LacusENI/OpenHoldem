#include "GameController.h"

#include <optional>

#include "Event.hpp"
#include "EventBus.hpp"
#include "GameModel.h"
#include "GameTypes.hpp"

namespace poker {
void GameController::subscribeEvent() {
    event_bus.subscribe<Event::StartGame>(&GameController::handle, this);
    event_bus.subscribe<Event::PlayerJoinRequest>(&GameController::handle, this);
    event_bus.subscribe<Event::PlayerAction>(&GameController::handle, this);
}

void GameController::handle(const Event::StartGame& event) {
    std::optional<Event::GameStarted> started = game_model.startGame();
    if (!started) return;
    event_bus.publish(*started);
    enterStage(Stage::PREFLOP);
}

void GameController::startNextHand() {
    std::optional<Event::GameStarted> started = game_model.startNextHand();
    if (!started) return;
    event_bus.publish(*started);
    enterStage(Stage::PREFLOP);
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    event_bus.publish(game_model.joinNewPlayer());
}

void GameController::handle(const Event::PlayerAction& event) {
    std::optional<Event::PlayerActed> acted = game_model.handlePlayerAction(event);
    if (!acted) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        if (auto waiting = game_model.awaitPlayerAction()) event_bus.publish(*waiting);
        return;
    }
    event_bus.publish(*acted);

    if (!game_model.isBettingRoundEnded()) {
        game_model.moveActingPlayer();
        if (auto waiting = game_model.awaitPlayerAction()) event_bus.publish(*waiting);
        return;
    }

    finishBettingRound(); // 退还未跟注筹码 + 重置下注轮 + 发布 BettingRoundEnded

    switch (decideAfterBettingRound()) {
    case AfterRound::HandOver:
        event_bus.publish(game_model.finishGame()); // 冗余防御：无人在局
        break;
    case AfterRound::UncontestedSettle:
        enterUncontestedSettle();
        break;
    case AfterRound::RunoutShowdown:
        enterStage(Stage::SHOWDOWN);
        break;
    case AfterRound::NextStreet:
        enterStage(next_stage(game_model.getCurrentStage()));
        break;
    }
}

GameController::AfterRound GameController::decideAfterBettingRound() const {
    if (game_model.isNoOnePlayerAlive()) return AfterRound::HandOver;            // 防御
    if (game_model.isOnlyOnePlayerAlive()) return AfterRound::UncontestedSettle; // 无人竞争
    if (game_model.isStreetsFinished()) return AfterRound::RunoutShowdown;       // 已无可下注者
    if (game_model.getCurrentStage() == Stage::RIVER) return AfterRound::RunoutShowdown;
    return AfterRound::NextStreet;
}

void GameController::enterStage(Stage stage) {
    event_bus.publish(game_model.changeStage(stage)); // 发布 StageChanged
    switch (stage) {
    case Stage::PREFLOP: enterPreflop(); break;
    case Stage::FLOP:
    case Stage::TURN:
    case Stage::RIVER: enterPostflop(); break;
    case Stage::SHOWDOWN: enterShowdown(); break;
    default: break; // SETTLEMENT 由各自的结算方法负责
    }
}

void GameController::enterPreflop() {
    game_model.shuffleDeck();
    for (const auto& event : game_model.dealHoleCards()) event_bus.publish(event);
    event_bus.publish(game_model.startBettingRound());
    event_bus.publish(game_model.postSmallBlind());
    event_bus.publish(game_model.postBigBlind());
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction()) event_bus.publish(*waiting);
}

void GameController::enterPostflop() {
    event_bus.publish(game_model.dealCommunityCards());
    event_bus.publish(game_model.startBettingRound());
    game_model.startActionRound();
    if (auto waiting = game_model.awaitPlayerAction()) event_bus.publish(*waiting);
}

void GameController::enterShowdown() {
    event_bus.publish(game_model.dealCommunityCards()); // 补发剩余公共牌（已满则空操作）
    for (const auto& event : game_model.revealHoleCards()) event_bus.publish(event);
    enterSettlement();
}

void GameController::enterSettlement() {
    event_bus.publish(game_model.changeStage(Stage::SETTLEMENT));
    event_bus.publish(game_model.splitPot());
    for (const auto& event : game_model.distributePots()) event_bus.publish(event);
    event_bus.publish(game_model.finishGame());
}

void GameController::enterUncontestedSettle() {
    event_bus.publish(game_model.changeStage(Stage::SETTLEMENT));
    event_bus.publish(game_model.settleUncontested());
    event_bus.publish(game_model.finishGame());
}

void GameController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    if (auto uncalled = game_model.returnUncalledBet()) event_bus.publish(*uncalled);
    event_bus.publish(game_model.closeBettingRound());
}
} // namespace poker
