#include "GameController.h"

#include "BettingRound.h"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameModel.h"
#include "GlobalLogger.h"
#include "SeatManager.h"

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
    enterStage(Stage::PREFLOP);
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    GlobalLogger::debug("[GameController] EventListener(PlayerJoinRequest)");
    game_model->joinNewPlayer();
    // if (game_model->getSeatManager()->getPlayerCount() == 3) {
    //     event_bus->publish(Event::StartGame{});
    // }
}

void GameController::handle(const Event::PlayerAction& event) {
    GlobalLogger::debug("[GameController] EventListener(PlayerActionSubmitted)");
    game_model->processPlayerInput(event);
    if (game_model->isRoundFinished()) {
        handleRoundEnded();
    } else {
        game_model->advanceToNextActor();
        awaitPlayerInput();
    }
}

void GameController::startBettingRound() {
    game_model->setupRoundStart();
    event_bus->publish(Event::BettingRoundStarted{});
    if (game_model->getCurrentStage() == Stage::PREFLOP) {
        game_model->postForcedBlind();
    }
    awaitPlayerInput();
}

void GameController::awaitPlayerInput() {
    game_model->awaitPlayerInput();
}

void GameController::handleRoundEnded() {
    event_bus->publish(Event::BettingRoundEnded{});
    Stage stage = game_model->getCurrentStage();
    enterStage(next_stage(stage));
}

void GameController::processShowdown() {
    game_model->revealHoleCards();
}

void GameController::processSettlement() {
    event_bus->publish(Event::GameEnded{});
}

void GameController::enterStage(Stage stage) {
    game_model->setCurrentStage(stage);
    event_bus->publish(Event::StageChanged{stage});
    if (stage == Stage::IDLE) {
    } else if (stage == Stage::PREFLOP) {
        game_model->dealHoleCards();
        startBettingRound();
    } else if (stage == Stage::SHOWDOWN) {
        processShowdown();
        enterStage(Stage::SETTLEMENT);
    } else if (stage == Stage::SETTLEMENT) {
        processSettlement();
    } else {
        game_model->dealCommunityCards();
        startBettingRound();
    }
}
} // namespace poker
