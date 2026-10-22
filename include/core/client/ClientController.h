#pragma once

#include "Event.hpp"
#include "ServerGateway.h"

namespace poker {
class EventBus;
class ClientModel;
class ClientView;

class ClientController {
private:
    EventBus& event_bus;
    ClientModel& client_model;
    ClientView& client_view;
    ServerGateway& server_gateway;

public:
    ClientController(EventBus& event_bus, ClientModel& client_model, ClientView& client_view,
                     ServerGateway& server_gateway)
        : event_bus(event_bus),
          client_model(client_model),
          client_view(client_view),
          server_gateway(server_gateway) {}
    ~ClientController() = default;

    void subscribeEvent();
    void handle(const Event::JoinResponse& event);
    void handle(const Event::PlayerJoined& event);
    void handle(const Event::GameStarted& event);
    void handle(const Event::HoleCardsDealt& event);
    void handle(const Event::BlindPosted& event);
    void handle(const Event::CommunityCardsDealt& event);
    void handle(const int event);
    void handle(const Event::TurnStarted& event);
    void handle(const Event::PlayerActed& event);
    void handle(const Event::HoleCardsRevealed& event);
};
} // namespace poker