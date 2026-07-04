#include <spdlog/spdlog.h>

#include "BettingRound.h"
#include "Client.h"
#include "ClientController.h"
#include "ClientModel.h"
#include "ConsoleView.h"
#include "Deck.h"
#include "Event.h"
#include "EventBus.h"
#include "GameController.h"
#include "GameModel.h"
#include "PotManager.h"
#include "SeatManager.h"
#include "Server.h"

int main() {
    using namespace poker;

    // Server
    auto server_event_bus = std::make_shared<EventBus>();
    auto deck = std::make_unique<Deck>();
    auto seat_manager = std::make_shared<SeatManager>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>();
    auto game_model = std::make_unique<GameModel>(server_event_bus, std::move(deck), pot_manager,
                                                  seat_manager, std::move(betting_round));
    auto game_controller = std::make_unique<GameController>(server_event_bus, std::move(game_model));

    // Client 0
    auto client_event_bus_0 = std::make_shared<EventBus>();
    auto client_model_0 = std::make_unique<ClientModel>();
    auto console_view_0 = std::make_unique<ui::ConsoleView>();
    auto client_controller_0 = std::make_unique<ClientController>(client_event_bus_0, std::move(client_model_0), std::move(console_view_0));

    // Client 1
    auto client_event_bus_1 = std::make_shared<EventBus>();
    auto client_model_1 = std::make_unique<ClientModel>();
    auto console_view_1 = std::make_unique<ui::ConsoleView>();
    auto client_controller_1 = std::make_unique<ClientController>(client_event_bus_1, std::move(client_model_1), std::move(console_view_1));

    std::vector client_event_bus_list = {client_event_bus_0, client_event_bus_1};

    auto server = std::make_unique<Server>(server_event_bus, client_event_bus_list);
    auto client_0 = std::make_unique<Client>(client_event_bus_0, server_event_bus);
    auto client_1 = std::make_unique<Client>(client_event_bus_1, server_event_bus);

    server_event_bus->subscribe<Event::PlayerJoined>([](const Event::PlayerJoined& event) {
        spdlog::info(std::format("[Game] Player Joined | new_seat={}", event.new_seat));
    });
    server_event_bus->subscribe<Event::GameStarted>([](const Event::GameStarted& event) {
        spdlog::info(std::format("[Game] Game Started | btn_seat={} big_blind={}", event.btn_seat,
                                 event.big_blind));
    });
    server_event_bus->subscribe<Event::GameEnded>(
        [](const Event::GameEnded&) { spdlog::info("[Game] Game Ended"); });
    server_event_bus->subscribe<Event::StageChanged>([](const Event::StageChanged& event) {
        spdlog::info(std::format("[Game] Stage Changed | stage={}", to_string(event.stage)));
    });
    server_event_bus->subscribe<Event::HoleCardsDealt>([](const Event::HoleCardsDealt& event) {
        spdlog::info(std::format("[Game] Hole Cards Dealt | player_seat={} hole_cards={}",
                                 event.player_seat, to_string(event.hole_cards)));
    });
    server_event_bus->subscribe<Event::CommunityCardsDealt>([](const Event::CommunityCardsDealt& event) {
        spdlog::info(std::format("[Game] Community Cards Dealt | community_cards={}",
                                 to_string(event.community_cards)));
    });
    server_event_bus->subscribe<Event::BlindPosted>([](const Event::BlindPosted& event) {
        std::string blind_type = event.is_big_blind ? "Big Blind" : "Small Blind";
        spdlog::info(std::format(
            "[Game] Blind Posted({}) | seat={} amount={} pot={} player_chips={} player_bet={}",
            blind_type, event.seat, event.amount, event.pot, event.player_chips, event.player_bet));
    });
    server_event_bus->subscribe<Event::BettingRoundStarted>([](const Event::BettingRoundStarted& event) {
        spdlog::info(std::format("[Game] Betting Round Started"));
    });
    server_event_bus->subscribe<Event::BettingRoundEnded>([](const Event::BettingRoundEnded& event) {
        spdlog::info(std::format("[Game] Betting Round Ended"));
    });
    server_event_bus->subscribe<Event::AwaitPlayerInput>([](const Event::AwaitPlayerInput& event) {
        spdlog::info(
            std::format("[Game] Await Player Input | player_seat={} player_chips={} player_bet={} "
                        "round_bet={} min_raise_delta={}",
                        event.player_seat, event.player_chips, event.player_bet, event.round_bet,
                        event.min_raise_delta));
    });
    server_event_bus->subscribe<Event::PlayerActed>([](const Event::PlayerActed& event) {
        spdlog::info(
            std::format("[Game] Player Acted | actor_seat={} action={} pot={} player_chips={} "
                        "player_bet={}",
                        event.actor_seat, to_string(event.action), event.pot, event.player_chips,
                        event.player_bet));
    });
    server_event_bus->subscribe<Event::HoleCardsRevealed>([](const Event::HoleCardsRevealed& event) {
        spdlog::info("[Game] Hole Cards Revealed | ...");
    });

    game_controller->subscribeEvent();
    client_controller_0->subscribeEvent();
    client_controller_1->subscribeEvent();

    server->subscribeEvent();
    client_0->subscribeEvent();
    client_1->subscribeEvent();

    client_controller_0->joinGame();
    client_controller_1->joinGame();
    game_controller->startGame();
    return 0;
}