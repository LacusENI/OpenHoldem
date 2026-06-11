#include "spdlog/spdlog.h"

#include "BettingRound.h"
#include "Deck.h"
#include "GameModel.h"
#include "GameController.h"
#include "PotManager.h"
#include "SeatingManager.h"
#include "Event.h"
#include "EventBus.h"

int main() {
    using namespace poker;

    auto event_bus = std::make_shared<EventBus>();
    auto deck = std::make_unique<Deck>();
    auto seating_manager = std::make_shared<SeatingManager>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>();
    auto game_model = std::make_unique<GameModel>(event_bus,
        std::move(deck), pot_manager, seating_manager, std::move(betting_round));
    GameController game_controller(event_bus, std::move(game_model));

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
    event_bus->subscribe<Event::StreetStarted>([](const Event::StreetStarted& event) {
        spdlog::info(std::format("Street Started: {}", to_string(event.street)));
    });
    event_bus->subscribe<Event::HoleCardsDealt>([](const Event::HoleCardsDealt& event) {
       spdlog::info(std::format("Hole Cards Dealt: Player#{} -> {} {}",
           event.player_seat, to_string(event.hole_cards[0]), to_string(event.hole_cards[1])));
    });
    event_bus->subscribe<Event::BlindPosted>([](const Event::BlindPosted& event) {
       std::string blind_type = event.is_big_blind ? "Big Blind" : "Small Blind";
       spdlog::info(std::format("Blind Posted: Player#{} Chips={} ({})", event.seat, event.amount, blind_type));
    });

    game_controller.startGame();

    return 0;
}