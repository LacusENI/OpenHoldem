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
    while (game_model->getStreet() != Street::AWARD) {
        event_bus->publish(Event::StreetStarted{
            game_model->getStreet()
        });
        // 发放公共牌
        game_model->dealCommunityCards();
        // 移至下一阶段
        game_model->advanceToNextStreet();
    }
}


} // namespace poker
