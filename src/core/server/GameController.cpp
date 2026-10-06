#include "GameController.h"


#include "Event.hpp"
#include "EventBus.hpp"
#include "GameModel.h"
#include "GameTypes.hpp"

namespace poker {
GameController::GameController(EventBus* event_bus, GameModel* model)
    : event_bus(event_bus), game_model(model) {}

void GameController::subscribeEvent() {
    event_bus->subscribe<Event::StartGame>(&GameController::handle, this);
    event_bus->subscribe<Event::PlayerJoinRequest>(&GameController::handle, this);
    event_bus->subscribe<Event::PlayerAction>(&GameController::handle, this);
}

void GameController::handle(const Event::StartGame& event) {
    if (!game_model->startGame()) return;
    game_model->changeStage(Stage::PREFLOP);
    processPreflopStage();
}

void GameController::startNextHand() {
    if (!game_model->startNextHand()) return;
    game_model->changeStage(Stage::PREFLOP);
    processPreflopStage();
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    game_model->joinNewPlayer();
}

void GameController::handle(const Event::PlayerAction& event) {
    if (!game_model->handlePlayerAction(event)) {
        // 无效行动：保持当前行动者不变，重新请求其行动
        game_model->awaitPlayerAction();
        return;
    }
    if (game_model->isBettingRoundEnded()) {
        finishBettingRound();

        // 冗余防御：正常流程至少有一名在局玩家可争夺底池，避免进入无法结算的状态
        if (game_model->isNoOnePlayerAlive()) {
            game_model->finishGame();
            return;
        }

        if (game_model->isOnlyOnePlayerAlive()) {
            handleUncontestedPot();
        } else if (game_model->isStreetsFinished()) {
            handleStreetsFinished();
        } else {
            advanceToNextStage();
        }
    } else {
        game_model->moveActingPlayer();
        game_model->awaitPlayerAction();
    }
}

void GameController::finishBettingRound() {
    // 先处理 Uncalled Bet（依赖本轮的 bet 数据），再重置下注轮状态
    game_model->returnUncalledBet();
    game_model->closeBettingRound();
}

void GameController::handleUncontestedPot() {
    // 无人竞争：不切分底池，整池直接判给唯一存活玩家
    game_model->changeStage(Stage::SETTLEMENT);
    game_model->settleUncontested();
    game_model->finishGame();
}

void GameController::handleStreetsFinished() {
    game_model->changeStage(Stage::SHOWDOWN);
    game_model->dealCommunityCards();
    processShowdownStage();
}

void GameController::advanceToNextStage() {
    if (game_model->getCurrentStage() == Stage::RIVER) {
        game_model->changeStage(Stage::SHOWDOWN);
        processShowdownStage();
    } else {
        game_model->advanceStage();
        processPostflopStage();
    }
}

void GameController::processPreflopStage() {
    game_model->shuffleDeck();
    game_model->dealHoleCards();
    game_model->startBettingRound();
    game_model->postSmallBlind();
    game_model->postBigBlind();
    game_model->startActionRound();
    game_model->awaitPlayerAction();
}

void GameController::processPostflopStage() {
    game_model->dealCommunityCards();
    game_model->startBettingRound();
    game_model->startActionRound();
    game_model->awaitPlayerAction();
}

void GameController::processShowdownStage() {
    game_model->setCurrentStage(Stage::SHOWDOWN);
    game_model->revealAllHoleCards();
    game_model->setCurrentStage(Stage::SETTLEMENT);
    game_model->splitPot();
    game_model->distributePots();
    game_model->finishGame();
}
} // namespace poker
