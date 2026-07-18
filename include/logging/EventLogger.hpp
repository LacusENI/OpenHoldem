#pragma once
#include <spdlog/spdlog.h>

#include <memory>

#include "Event.hpp"
#include "EventBus.hpp"

namespace poker::logging {

class EventLogger {
private:
    EventBus* event_bus;
    std::string logger_name;

public:
    explicit EventLogger(EventBus* event_bus, const std::string& logger_name)
        : event_bus(event_bus), logger_name(logger_name) {}
    ~EventLogger() = default;
    void subscribeEvent() {
        event_bus->subscribe<Event::NewPlayerJoined>([this](const Event::NewPlayerJoined& event) {
            spdlog::info(std::format("[{}][Game] New Player Joined | new_seat={}", logger_name,
                                     event.new_seat));
        });
        event_bus->subscribe<Event::GameStarted>([this](const Event::GameStarted& event) {
            spdlog::info(std::format("[{}][Game] Game Started | btn_seat={} big_blind={}",
                                     logger_name, event.btn_seat, event.big_blind));
        });
        event_bus->subscribe<Event::GameEnded>([this](const Event::GameEnded&) {
            spdlog::info("[{}][Game] Game Ended", logger_name);
        });
        event_bus->subscribe<Event::StageChanged>([this](const Event::StageChanged& event) {
            spdlog::info(std::format("[{}][Game] Stage Changed | stage={}", logger_name,
                                     to_string(event.stage)));
        });
        event_bus->subscribe<Event::HoleCardsDealt>([this](const Event::HoleCardsDealt& event) {
            spdlog::info(std::format("[{}][Game] Hole Cards Dealt | player_seat={} hole_cards={}",
                                     logger_name, event.player_seat, to_string(event.hole_cards)));
        });
        event_bus->subscribe<Event::CommunityCardsDealt>(
            [this](const Event::CommunityCardsDealt& event) {
                spdlog::info(std::format("[{}][Game] Community Cards Dealt | community_cards={}",
                                         logger_name, to_string(event.community_cards)));
            });
        event_bus->subscribe<Event::BlindPosted>([this](const Event::BlindPosted& event) {
            std::string blind_type = event.is_big_blind ? "Big Blind" : "Small Blind";
            spdlog::info(
                std::format("[{}][Game] Blind Posted({}) | seat={} amount={} pot={} "
                            "player_chips={} player_bet={}",
                            logger_name, blind_type, event.seat, event.amount, event.pot,
                            event.player_chips, event.player_bet));
        });
        event_bus->subscribe<Event::BettingRoundStarted>(
            [this](const Event::BettingRoundStarted& event) {
                spdlog::info(std::format("[{}][Game] Betting Round Started", logger_name));
            });
        event_bus->subscribe<Event::BettingRoundEnded>(
            [this](const Event::BettingRoundEnded& event) {
                spdlog::info(std::format("[{}][Game] Betting Round Ended", logger_name));
            });
        event_bus->subscribe<Event::AwaitPlayerAction>(
            [this](const Event::AwaitPlayerAction& event) {
                spdlog::info(std::format(
                    "[{}][Game] Await Player Action | player_seat={} player_chips={} player_bet={} "
                    "round_bet={} min_raise_delta={}",
                    logger_name, event.player_seat, event.player_chips, event.player_bet,
                    event.round_bet, event.min_raise_delta));
            });
        event_bus->subscribe<Event::PlayerActed>([this](const Event::PlayerActed& event) {
            spdlog::info(std::format(
                "[{}][Game] Player Acted | actor_seat={} action={} pot={} player_chips={} "
                "player_bet={}",
                logger_name, event.actor_seat, to_string(event.action), event.pot,
                event.player_chips, event.player_bet));
        });
        event_bus->subscribe<Event::HoleCardsRevealed>(
            [this](const Event::HoleCardsRevealed& event) {
                spdlog::info("[{}][Game] Hole Cards Revealed | ...", logger_name);
            });
    }
};

} // namespace poker::logging