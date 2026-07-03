#include <spdlog/spdlog.h>

#include "BettingRound.h"
#include "ClientModel.h"
#include "ClientView.h"
#include "ConsoleView.h"
#include "Deck.h"
#include "Event.h"
#include "EventBus.h"
#include "GameController.h"
#include "GameModel.h"
#include "PotManager.h"
#include "SeatManager.h"

int main() {
    using namespace poker;

    auto event_bus = std::make_shared<EventBus>();
    auto deck = std::make_unique<Deck>();
    auto seat_manager = std::make_shared<SeatManager>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>();
    auto game_model = std::make_unique<GameModel>(event_bus, std::move(deck), pot_manager,
                                                  seat_manager, std::move(betting_round));
    GameController game_controller(event_bus, std::move(game_model));

    seat_manager->addNewPlayer();
    seat_manager->addNewPlayer();

    event_bus->subscribe<Event::GameStarted>(
        [](const Event::GameStarted& event) { spdlog::info(std::format("[Game] Game Started")); });
    event_bus->subscribe<Event::GameEnded>(
        [](const Event::GameEnded&) { spdlog::info("[Game] Game Ended"); });
    event_bus->subscribe<Event::StageChanged>([](const Event::StageChanged& event) {
        spdlog::info(std::format("[Game] Stage Changed | stage={}", to_string(event.stage)));
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
        spdlog::info(std::format(
            "[Game] Blind Posted({}) | seat={} amount={} pot={} player_chips={} player_bet={}",
            blind_type, event.seat, event.amount, event.pot, event.player_chips, event.player_bet));
    });
    event_bus->subscribe<Event::BettingRoundStarted>([](const Event::BettingRoundStarted& event) {
        spdlog::info(std::format("[Game] Betting Round Started"));
    });
    event_bus->subscribe<Event::BettingRoundEnded>([](const Event::BettingRoundEnded& event) {
        spdlog::info(std::format("[Game] Betting Round Ended"));
    });
    event_bus->subscribe<Event::AwaitPlayerInput>([](const Event::AwaitPlayerInput& event) {
        spdlog::info(
            std::format("[Game] Await Player Input | actor_seat={} player_chips={} player_bet={} "
                        "round_bet={} min_raise_delta={}",
                        event.actor_seat, event.player_chips, event.player_bet, event.round_bet,
                        event.min_raise_delta));
    });
    event_bus->subscribe<Event::PlayerActed>([](const Event::PlayerActed& event) {
        spdlog::info(
            std::format("[Game] Player Acted | actor_seat={} action={} pot={} player_chips={} "
                        "player_bet={}",
                        event.actor_seat, to_string(event.action), event.pot, event.player_chips,
                        event.player_bet));
    });
    event_bus->subscribe<Event::HoleCardsRevealed>([](const Event::HoleCardsRevealed& event) {
        spdlog::info("[Game] Hole Cards Revealed | ...");
    });
    game_controller.subscribeEvents();
    game_controller.startGame();

    ClientModel client_model;
    client_model.players = {Player{.position = 0, .hole_cards = {Card("C8"), Card("CA")}, .chips = 50},Player{.position = 1, .chips = 20}};
    client_model.this_player_seat = 0;
    client_model.pot = 20;
    client_model.player_bets = {5, 15};
    client_model.player_states = {PlayerState::BET, PlayerState::RAISED};
    client_model.community_cards = {Card("DT"), Card("DA"), Card("HT")};
    ui::ConsoleView console_view;
    console_view.render(client_model);
    return 0;
}