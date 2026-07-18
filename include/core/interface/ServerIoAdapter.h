#pragma once

#include "Event.hpp"
#include "EventBus.hpp"
#include "GlobalLogger.h"

namespace poker {
class ServerIoAdapter {
private:
    EventBus* event_bus;

public:
    ServerIoAdapter(EventBus* event_bus) : event_bus(event_bus) {}
    ~ServerIoAdapter() = default;

    std::function<void(const GameEvent&)> sendToAllClient;
    std::function<void(int, const GameEvent&)> sendToOneClient;

    void subscribeEvent() {
        event_bus->subscribe<NetworkEvent::ClientConnected>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<NetworkEvent::ClientDisconnected>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::PlayerJoinResponse>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::GameStarted>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::GameEnded>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::BlindPosted>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::CommunityCardsDealt>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::BettingRoundStarted>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::BettingRoundEnded>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::PlayerActed>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::HoleCardsDealt>(&ServerIoAdapter::handle, this);
        event_bus->subscribe<Event::AwaitPlayerAction>(&ServerIoAdapter::handle, this);
    }
    void publishGameEventReceived(const GameEvent& event);
    void handle(const NetworkEvent::ClientConnected& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(ClientConnected)");
        event_bus->publish(Event::PlayerJoinRequest{});
    }
    void handle(const NetworkEvent::ClientDisconnected& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(ClientDisconnected)");
    }
    void handle(const Event::PlayerJoinResponse& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(PlayerJoinResponse)");
        sendToOneClient(event.player_seat, event);
    }
    void handle(const Event::GameStarted& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(GameStarted)");
        sendToAllClient(event);
    }
    void handle(const Event::GameEnded& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(GameEnded)");
        sendToAllClient(event);
    }
    void handle(const Event::BlindPosted& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(BlindPosted)");
        sendToAllClient(event);
    }
    void handle(const Event::CommunityCardsDealt& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(CommunityCardsDealt)");
        sendToAllClient(event);
    }
    void handle(const Event::BettingRoundStarted& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(BettingRoundStarted)");
        sendToAllClient(event);
    }
    void handle(const Event::BettingRoundEnded& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(BettingRoundEnded)");
        sendToAllClient(event);
    }
    void handle(const Event::PlayerActed& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(PlayerActed)");
        sendToAllClient(event);
    }
    void handle(const Event::HoleCardsDealt& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(HoleCardsDealt)");
        sendToOneClient(event.player_seat, event);
    }
    void handle(const Event::AwaitPlayerAction& event) {
        GlobalLogger::debug("[ServerIoAdapter] EventListener(AwaitPlayerAction)");
        sendToOneClient(event.player_seat, event);
    }
};
} // namespace poker