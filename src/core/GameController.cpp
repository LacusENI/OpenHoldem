#include "GameController.h"
#include "Event.h"
#include "EventBus.h"
#include "BettingRound.h"
#include "GameModel.h"

namespace poker {
GameController::GameController(
    std::shared_ptr<EventBus> event_bus,
    std::unique_ptr<GameModel> model)
    : event_bus(std::move(event_bus)), game_model(std::move(model)) {}

GameController::~GameController() = default;

void GameController::startGame() {
    game_model->startGame();
    game_model->dealHoleCards();
    game_model->enterPreflopStreet();
    startStreet();
}

void GameController::startStreet() {
    event_bus->publish(Event::StreetStarted{
        game_model->getStreet()
    });
    if (game_model->getStreet() == Street::PREFLOP) {
        game_model->postForcedBlind();
    }
    // 发放公共牌
    game_model->dealCommunityCards();
    // 开始下注轮
    startBettingRound();
}

void GameController::startBettingRound() {
    game_model->startBettingRound();
    awaitPlayerInput();
    finishBettingRound();
}

void GameController::awaitPlayerInput() {
    game_model->awaitPlayerInput();
}

void GameController::finishBettingRound() {
    advanceToNextStreet();
}

void GameController::advanceToNextStreet() {
    game_model->advanceToNextStreet();
    if (game_model->getStreet() != Street::AWARD) {
        startStreet();
    }
    else {
        enterShowdownStreet();
    }
}

void GameController::enterShowdownStreet() {
    game_model->processShowdown();
}
} // namespace poker
