#include "GameController.h"

#include <vector>

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
    game_model->startGame();
    processPreflopStage();
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    game_model->joinNewPlayer();
}

void GameController::handle(const Event::PlayerAction& event) {
    game_model->handlePlayerAction(event);
    if (game_model->isBettingRoundEnded()) {
        if (game_model->isOnlyOnePlayerAlive()) {
            processShowdownStage();
        } else { // 进入下一个阶段
            if (game_model->getCurrentStage() == Stage::RIVER) {
                processShowdownStage();
            } else {
                processPostflopStage();
            }
        }
    } else {
        game_model->moveActingPlayer();
        game_model->awaitPlayerAction();
    }
}

void GameController::processPreflopStage() {
    game_model->changeStage(Stage::PREFLOP);
    game_model->shuffleDeck();
    game_model->dealHoleCards();
    game_model->startBettingRound();
    game_model->postSmallBlind();
    game_model->postBigBlind();
    game_model->awaitPlayerAction();
}

void GameController::processPostflopStage() {
    Stage stage = game_model->getCurrentStage();
    game_model->changeStage(next_stage(stage));
    game_model->dealCommunityCards();
    game_model->startBettingRound();
    game_model->awaitPlayerAction();
}

void GameController::processShowdownStage() {
    game_model->setCurrentStage(Stage::SHOWDOWN);
    game_model->revealHoleCards();
    auto winners = game_model->comparePlayerHands();
    processSettlementStage(winners);
}

void GameController::processSettlementStage(std::vector<seat_t> winners) {
    game_model->setCurrentStage(Stage::SETTLEMENT);
    game_model->awardPotToWinners(winners);
    game_model->finishGame();
}
} // namespace poker
