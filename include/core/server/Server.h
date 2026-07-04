#pragma once
#include <memory>
#include <vector>

#include "Event.h"
#include "EventBus.h"

namespace poker {
class Server {
private:
    std::shared_ptr<EventBus> server_event_bus;
    std::vector<std::shared_ptr<EventBus>> client_event_bus_list;

public:
    Server(std::shared_ptr<EventBus> server_event_bus,
           const std::vector<std::shared_ptr<EventBus>>& client_event_bus_list)
        : server_event_bus(std::move(server_event_bus)),
          client_event_bus_list(client_event_bus_list) {};
    ~Server() = default;
    void subscribeEvent() {
        server_event_bus->subscribe<Event::PlayerJoinSucceeded>(
            [this](const auto& event) { this->sendToClient(event.player_seat, event); });
        server_event_bus->subscribe<Event::PlayerJoined>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::GameStarted>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::HoleCardsDealt>(
            [this](const auto& event) { this->sendToClient(event.player_seat, event); });
        server_event_bus->subscribe<Event::BlindPosted>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::CommunityCardsDealt>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::BettingRoundStarted>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::BettingRoundEnded>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::PlayerActed>(
            [this](const auto& event) { this->broadcastToAllClient(event); });
        server_event_bus->subscribe<Event::AwaitPlayerInput>(
            [this](const auto& event) { this->sendToClient(event.player_seat, event); });
    }

    template <typename T>
    void broadcastToAllClient(const T& event) {
        for (const auto& client : client_event_bus_list) {
            client->publish(event);
        }
    }

    template <typename T>
    void sendToClient(int seat_idx, const T& event) {
        client_event_bus_list.at(seat_idx)->publish(event);
    }
};
} // namespace poker