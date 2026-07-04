#include "GameController.h"

#include "BettingRound.h"
#include "Event.h"
#include "EventBus.h"
#include "GameModel.h"

namespace poker {
GameController::GameController(std::shared_ptr<EventBus> event_bus,
                               std::unique_ptr<GameModel> model)
    : event_bus(std::move(event_bus)), game_model(std::move(model)) {}

GameController::~GameController() = default;

void GameController::subscribeEvent() {
    event_bus->subscribe<Event::PlayerInputSubmitted>(
        [this](const auto& event) { this->handlePlayerInput(event); });
}

void GameController::startGame() {
    game_model->startGame();
    enterStage(Stage::PREFLOP);
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


void GameController::handlePlayerInput(const Event::PlayerInputSubmitted& event) {
    game_model->processPlayerInput(event);
    if (game_model->isRoundFinished()) {
        handleRoundEnded();
    } else {
        game_model->advanceToNextActor();
        awaitPlayerInput();
    }
}

void GameController::handleRoundEnded() {
    event_bus->publish(Event::BettingRoundEnded{});
    Stage stage = game_model->getCurrentStage();
    enterStage(next_stage(stage));
}

void GameController::processShowdown() {
    game_model->revealHoleCards();
}

void GameController::processSettlement() {}

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
        // TODO
    } else {
        game_model->dealCommunityCards();
        startBettingRound();
    }
}
} // namespace poker
