#include "spdlog/spdlog.h"

#include "BettingRound.h"
#include "Deck.h"
#include "GameModel.h"
#include "GameController.h"
#include "PotManager.h"
#include "SeatingManager.h"
#include "Event.h"
#include "EventBus.h"

using namespace poker;

int main() {
    auto event_bus = std::make_unique<EventBus>();
    auto deck = std::make_unique<Deck>();
    auto seating_manager = std::make_shared<SeatingManager>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>();
    auto game_model = std::make_unique<GameModel>(
        std::move(deck), pot_manager, seating_manager, std::move(betting_round));
    GameController game(std::move(game_model));
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    event_bus->subscribe<Event::GameStarted>([](const Event::GameStarted& event) {
       spdlog::info("Game Started");
    });
    event_bus->subscribe<Event::GameEnded>([](const Event::GameEnded&) {
       spdlog::info("Game Ended");
    });
    event_bus->publish(Event::GameStarted{});
    game.run();
    event_bus->publish(Event::GameEnded{});
    return 0;
}