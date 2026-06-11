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
    event_bus->publish(Event::GameStarted{});
    game_model->startGame();
    game_model->dealHoleCards();
    enterPreflopStreet();
}

void GameController::enterPreflopStreet() {
    game_model->enterPreflopStreet();
    game_model->postForcedBlind();
}
} // namespace poker
