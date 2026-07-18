#pragma once
#include <spdlog/spdlog.h>

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
        subscribeGameEvent();
        subscribeNetworkEvent();
    }

    void subscribeNetworkEvent() {
        event_bus->subscribe<NetworkEvent::ClientConnected>([this](const auto& event) {
            spdlog::info(std::format("[{}][Network] Client Connected | client_id={}", logger_name,
                                     event.client_id));
        });
        event_bus->subscribe<NetworkEvent::ClientDisconnected>([this](const auto& event) {
            spdlog::info(std::format("[{}][Network] Client Disconnected | client_id={}",
                                     logger_name, event.client_id));
        });
        event_bus->subscribe<NetworkEvent::ServerDisconnected>([this](const auto& event) {
            spdlog::info(std::format("[{}][Network] Server Disconnected", logger_name));
        });
    }

    void subscribeGameEvent() {
        event_bus->subscribe<Event::StartGame>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Start Game |", logger_name));
        });
        event_bus->subscribe<Event::NewPlayerJoined>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] New Player Joined | new_seat={}", logger_name,
                                     event.new_seat));
        });
        event_bus->subscribe<Event::PlayerJoinRequest>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Player Join Request | player_name=\"{}\"", logger_name,
                                     event.player_name));
        });
        event_bus->subscribe<Event::PlayerJoinResponse>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Player Join Response | player_seat={}",
                                     logger_name, event.player_seat));
        });
        event_bus->subscribe<Event::GameStarted>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Game Started | btn_seat={} big_blind={}",
                                     logger_name, event.btn_seat, event.big_blind));
        });
        event_bus->subscribe<Event::GameEnded>(
            [this](const auto&) { spdlog::info("[{}][Game] Game Ended", logger_name); });
        event_bus->subscribe<Event::StageChanged>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Stage Changed | stage={}", logger_name,
                                     to_string(event.stage)));
        });
        event_bus->subscribe<Event::HoleCardsDealt>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Hole Cards Dealt | player_seat={} hole_cards={}",
                                     logger_name, event.player_seat, to_string(event.hole_cards)));
        });
        event_bus->subscribe<Event::CommunityCardsDealt>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Community Cards Dealt | community_cards={}",
                                     logger_name, to_string(event.community_cards)));
        });
        event_bus->subscribe<Event::BlindPosted>([this](const auto& event) {
            std::string blind_type = event.is_big_blind ? "Big Blind" : "Small Blind";
            spdlog::info(
                std::format("[{}][Game] Blind Posted({}) | seat={} amount={} pot={} "
                            "player_chips={} player_bet={}",
                            logger_name, blind_type, event.seat, event.amount, event.pot,
                            event.player_chips, event.player_bet));
        });
        event_bus->subscribe<Event::BettingRoundStarted>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Betting Round Started", logger_name));
        });
        event_bus->subscribe<Event::BettingRoundEnded>([this](const auto& event) {
            spdlog::info(std::format("[{}][Game] Betting Round Ended", logger_name));
        });
        event_bus->subscribe<Event::AwaitPlayerAction>([this](const auto& event) {
            spdlog::info(std::format(
                "[{}][Game] Await Player Action | player_seat={} player_chips={} player_bet={} "
                "round_bet={} min_raise_delta={}",
                logger_name, event.player_seat, event.player_chips, event.player_bet,
                event.round_bet, event.min_raise_delta));
        });
        event_bus->subscribe<Event::PlayerActed>([this](const auto& event) {
            spdlog::info(std::format(
                "[{}][Game] Player Acted | actor_seat={} action={} pot={} player_chips={} "
                "player_bet={}",
                logger_name, event.actor_seat, to_string(event.action), event.pot,
                event.player_chips, event.player_bet));
        });
        event_bus->subscribe<Event::HoleCardsRevealed>([this](const auto& event) {
            spdlog::info("[{}][Game] Hole Cards Revealed | ...", logger_name);
        });
    }
};

} // namespace poker::logging