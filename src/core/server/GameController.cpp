#include "GameController.h"

#include "Event.hpp"
#include "EventBus.hpp"
#include "GameModel.h"

namespace poker {
GameController::GameController(EventBus* event_bus, GameModel* model)
    : event_bus(event_bus), game_model(model) {}

void GameController::subscribeEvent() {
    event_bus->subscribe<Event::StartGame>(&GameController::handle, this);
    event_bus->subscribe<Event::PlayerJoinRequest>(&GameController::handle, this);
    event_bus->subscribe<Event::PlayerAction>(&GameController::handle, this);
}

void GameController::handle(const Event::StartGame& event) {
    // game_model->startGame();
}

void GameController::handle(const Event::PlayerJoinRequest& event) {
    // GlobalLogger::debug("[GameController] EventListener(PlayerJoinRequest)");
    // game_model->joinNewPlayer();
    //  if (game_model->getSeatManager()->getPlayerCount() == 3) {
    //      event_bus->publish(Event::StartGame{});
    //  }
}

void GameController::handle(const Event::PlayerAction& event) {
    // GlobalLogger::debug("[GameController] EventListener(PlayerActionSubmitted)");
    // game_model->processPlayerInput(event);
}
} // namespace poker
