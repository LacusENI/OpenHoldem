#include <spdlog/spdlog.h>

#include "Event.hpp"
#include "EventBus.hpp"

int main() {
    using namespace poker;

    auto event_bus = std::make_unique<EventBus>();

    event_bus->subscribe<Event::NewPlayerJoined>([](const Event::NewPlayerJoined& event) {
        spdlog::info(std::format("[Game] New Player Joined | new_seat={}", event.new_seat));
    });
    event_bus->subscribe<Event::GameStarted>([](const Event::GameStarted& event) {
        spdlog::info(std::format("[Game] Game Started | btn_seat={} big_blind={}", event.btn_seat,
                                 event.big_blind));
    });
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
    event_bus->subscribe<Event::AwaitPlayerAction>([](const Event::AwaitPlayerAction& event) {
        spdlog::info(
            std::format("[Game] Await Player Action | player_seat={} player_chips={} player_bet={} "
                        "round_bet={} min_raise_delta={}",
                        event.player_seat, event.player_chips, event.player_bet, event.round_bet,
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

    return 0;
}