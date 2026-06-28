#include "spdlog/spdlog.h"

#include "BettingRound.h"
#include "Deck.h"
#include "GameModel.h"
#include "GameController.h"
#include "Pot.h"
#include "Table.h"
#include "Event.h"
#include "EventBus.h"

int main() {
    using namespace poker;

    auto event_bus = std::make_shared<EventBus>();
    auto deck = std::make_unique<Deck>();
    auto table = std::make_shared<Table>();
    auto pot_manager = std::make_shared<Pot>();
    auto betting_round = std::make_unique<BettingRound>();
    auto game_model = std::make_unique<GameModel>(event_bus,
        std::move(deck), pot_manager, table, std::move(betting_round));
    GameController game_controller(event_bus, std::move(game_model));

    table->addNewPlayer();
    table->addNewPlayer();

    event_bus->subscribe<Event::GameStarted>([](const Event::GameStarted& event) {
        spdlog::info(std::format("[Game] Game Started"));
    });
    event_bus->subscribe<Event::GameEnded>([](const Event::GameEnded&) {
        spdlog::info("[Game] Game Ended");
    });
    event_bus->subscribe<Event::StageChanged>([](const Event::StageChanged& event) {
        spdlog::info(std::format("[Game] Stage Changed | stage={}",
            to_string(event.stage)));
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
        spdlog::info(std::format("[Game] Betting Round Started"));
    });
    event_bus->subscribe<Event::BettingRoundEnded>([](const Event::BettingRoundEnded& event) {
        spdlog::info(std::format("[Game] Betting Round Ended"));
    });
    event_bus->subscribe<Event::AwaitPlayerInput>([](const Event::AwaitPlayerInput& event) {
        spdlog::info(std::format("[Game] Await Player Input | actor_seat={}",
           event.actor_seat));
    });
    event_bus->subscribe<Event::PlayerActed>([](const Event::PlayerActed& event) {
       spdlog::info(std::format("[Game] Player Acted | actor_seat={} action={}",
            event.actor_seat, to_string(event.action)
       ));
    });
    event_bus->subscribe<Event::HoleCardsRevealed>([](const Event::HoleCardsRevealed& event) {
        spdlog::info("[Game] Hole Cards Revealed | ...");
    });
    game_controller.subscribeEvents();
    game_controller.startGame();

    return 0;
}