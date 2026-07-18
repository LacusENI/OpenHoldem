#pragma once

#include "Event.hpp"

namespace poker {
class EventBus;
class ClientModel;
class ClientView;

class ClientController {
private:
    EventBus* event_bus;
    ClientModel* client_model;
    ClientView* client_view;

public:
    ClientController(EventBus* event_bus, ClientModel* client_model,
                     ClientView* client_view);
    ~ClientController() = default;

    void subscribeEvent();
    void joinGame();
    void onPlayerJoinSucceeded(const Event::PlayerJoinResponse& event);
    void onGameJoined(const Event::NewPlayerJoined& event);
    void onGameStarted(const Event::GameStarted& event);
    void onHoleCardsDealt(const Event::HoleCardsDealt& event);
    void onBlindPosted(const Event::BlindPosted& event);
    void onCommunityCardsDealt(const Event::CommunityCardsDealt& event);
    void onBettingRoundStarted(const Event::BettingRoundStarted& event);
    void onBettingRoundEnded(const Event::BettingRoundEnded& event);
    void onAwaitPlayerInput(const Event::AwaitPlayerAction& event);
    void onPlayerActed(const Event::PlayerActed& event);
    void onHoleCardsRevealed(const Event::HoleCardsRevealed& event);
};
} // namespace poker