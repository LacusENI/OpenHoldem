#pragma once

#include "Event.hpp"
#include "EventBus.hpp"
#include "GlobalLogger.h"

namespace poker {
class ClientIoAdapter {
private:
    EventBus* event_bus;

public:
    explicit ClientIoAdapter(EventBus* event_bus) : event_bus(event_bus) {}
    ~ClientIoAdapter() = default;

    std::function<void(const GameEvent&)> sendToServer;

    void subscribeEvent() {
        event_bus->subscribe<Event::PlayerActionSubmitted>(&ClientIoAdapter::handle, this);
    }
    void handle(const Event::PlayerActionSubmitted& event) {
        GlobalLogger::debug("[ClientIoAdapter] EventListener(PlayerActionSubmitted)");
        sendToServer(event);
    }
};
} // namespace poker