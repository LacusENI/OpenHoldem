#include "GameController.h"

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
    if (!game_model.startGame()) return;
    enterStage(Stage::PREFLOP);
}

void GameController::startNextHand() {
    if (!game_model.startNextHand()) return;
    enterStage(Stage::PREFLOP);
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    game_model.joinNewPlayer();
}

void GameController::handle(const Event::PlayerAction& event) {
    if (!game_model.handlePlayerAction(event)) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        game_model.awaitPlayerAction();
        return;
    }
    if (!game_model.isBettingRoundEnded()) {
        game_model.moveActingPlayer();
        game_model.awaitPlayerAction();
        return;
    }

    finishBettingRound(); // 退还未跟注筹码 + 重置下注轮 + 发布 BettingRoundEnded

    switch (decideAfterBettingRound()) {
    case AfterRound::HandOver:
        game_model.finishGame(); // 冗余防御：无人在局
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
    if (game_model.isNoOnePlayerAlive()) return AfterRound::HandOver;             // 防御
    if (game_model.isOnlyOnePlayerAlive()) return AfterRound::UncontestedSettle;  // 无人竞争
    if (game_model.isStreetsFinished()) return AfterRound::RunoutShowdown;        // 已无可下注者
    if (game_model.getCurrentStage() == Stage::RIVER) return AfterRound::RunoutShowdown;
    return AfterRound::NextStreet;
}

void GameController::enterStage(Stage stage) {
    game_model.changeStage(stage); // 发布 StageChanged
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
    game_model.dealHoleCards();
    game_model.startBettingRound();
    game_model.postSmallBlind();
    game_model.postBigBlind();
    game_model.startActionRound();
    game_model.awaitPlayerAction();
}

void GameController::enterPostflop() {
    game_model.dealCommunityCards();
    game_model.startBettingRound();
    game_model.startActionRound();
    game_model.awaitPlayerAction();
}

void GameController::enterShowdown() {
    game_model.dealCommunityCards(); // 补发剩余公共牌（已满 5 张则空操作）
    game_model.revealHoleCards();
    enterSettlement();
}

void GameController::enterSettlement() {
    game_model.changeStage(Stage::SETTLEMENT);
    game_model.splitPot();
    game_model.distributePots();
    game_model.finishGame();
}

void GameController::enterUncontestedSettle() {
    game_model.changeStage(Stage::SETTLEMENT);
    game_model.settleUncontested();
    game_model.finishGame();
}

void GameController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    game_model.returnUncalledBet();
    game_model.closeBettingRound();
}
} // namespace poker
