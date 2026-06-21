#include "GameController.h"
#include "Event.h"
#include "EventBus.h"
#include "BettingRound.h"
#include "GameModel.h"

namespace poker {
GameController::GameController(
    std::shared_ptr<EventBus> event_bus,
    std::unique_ptr<GameModel> model)
    : event_bus(std::move(event_bus)), game_model(std::move(model)) {
}

GameController::~GameController() = default;

void GameController::subscribe() {
    event_bus->subscribe<Event::AwaitPlayerInput>([this](const auto& event) {
        this->onAwaitPlayerInput(event);
    });
    event_bus->subscribe<Event::PlayerInputSubmitted>([this](const auto& event) {
        this->onPlayerInputSubmitted(event);
    });
}

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
    else {
        game_model->dealCommunityCards();
    }
    // 开始下注轮
    startBettingRound();
}

void GameController::startBettingRound() {
    game_model->startBettingRound();
    awaitPlayerInput();
}

void GameController::awaitPlayerInput() {
    game_model->awaitPlayerInput();
}

void GameController::onAwaitPlayerInput(const Event::AwaitPlayerInput& event) {
    event_bus->publish(Event::PlayerInputSubmitted{game_model->getBigBlind(), false});
}

void GameController::onPlayerInputSubmitted(const Event::PlayerInputSubmitted& event) {
    game_model->processPlayerInput(event);
    if (game_model->isRoundFinished()) {
        closeBettingRound();
    }
    else {
        game_model->advanceToNextActor();
        awaitPlayerInput();
    }
}

void GameController::closeBettingRound() {
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
