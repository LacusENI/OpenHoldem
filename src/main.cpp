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
        spdlog::info(std::format("[Game] Game Started | btn_seat={}",
            event.btn_seat));
    });
    event_bus->subscribe<Event::GameEnded>([](const Event::GameEnded&) {
        spdlog::info("[Game] Game Ended");
    });
    event_bus->subscribe<Event::StreetStarted>([](const Event::StreetStarted& event) {
        spdlog::info(std::format("[Game] Street Started | street={}",
            to_string(event.street)));
    });
    event_bus->subscribe<Event::HoleCardsDealt>([](const Event::HoleCardsDealt& event) {
        spdlog::info(std::format("[Game] Hole Cards Dealt | player_seat={} hole_cards={}",
           event.player_seat, to_string(event.hole_cards)));
    });
    event_bus->subscribe<Event::CommunityCardsDealt>([](const Event::CommunityCardsDealt& event) {
       spdlog::info(std::format("[Game] Community Cards Dealt | community_cards={}",
           to_string(event.community_cards)));
    });
    event_bus->subscribe<Event::BlindPosted>([](const Event::BlindPosted& event) {
        std::string blind_type = event.is_big_blind ? "Big Blind" : "Small Blind";
        spdlog::info(std::format("[Game] Blind Posted({}) | seat={} amount={} ",
           blind_type, event.seat, event.amount));
    });
    event_bus->subscribe<Event::BettingRoundStarted>([](const Event::BettingRoundStarted& event) {
        spdlog::info(std::format("[Game] Betting Round Started | first_actor_seat={}",
           event.first_actor_seat));
    });
    event_bus->subscribe<Event::AwaitPlayerInput>([](const Event::AwaitPlayerInput& event) {
        spdlog::info(std::format("[Game] Await Player Input | actor_seat={}",
           event.actor_seat));
    });
    event_bus->subscribe<Event::ShowdownCompleted>([](const Event::ShowdownCompleted& event) {
        spdlog::info("[Game] Showdown Completed");
    });
    game_controller.subscribe();
    game_controller.startGame();

    return 0;
}