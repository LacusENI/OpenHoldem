#pragma once
#include <memory>

#include "Event.hpp"
#include "EventBus.hpp"

namespace poker {
class Client {
private:
    std::shared_ptr<EventBus> client_event_bus;
    std::shared_ptr<EventBus> server_event_bus;

public:
    Client(std::shared_ptr<EventBus> client_event_bus, std::shared_ptr<EventBus> server_event_bus)
        : client_event_bus(std::move(client_event_bus)), server_event_bus(std::move(server_event_bus)) {}
    ~Client() = default;

    void subscribeEvent() {
        client_event_bus->subscribe<Event::PlayerJoining>(
            [this](const Event::PlayerJoining& event) { sendToServer(event); });
        client_event_bus->subscribe<Event::PlayerInputSubmitted>(
            [this](const Event::PlayerInputSubmitted& event) { sendToServer(event); });
    }

    template <typename T>
    void sendToServer(const T& event) {
        server_event_bus->publish(event);
    }
};

} // namespace poker