#pragma once
#include <memory>

#include "Event.h"

namespace poker {
class EventBus;
class ClientModel;
class ClientView;

class ClientController {
private:
    std::shared_ptr<EventBus> event_bus;
    std::unique_ptr<ClientModel> client_model;
    std::unique_ptr<ClientView> client_view;

public:
    ClientController(std::shared_ptr<EventBus> event_bus, std::unique_ptr<ClientModel> client_model,
                     std::unique_ptr<ClientView> client_view);
    ~ClientController();

    void subscribeEvent();
    void joinGame();
    void onPlayerJoinSucceeded(const Event::PlayerJoinSucceeded& event);
    void onGameJoined(const Event::PlayerJoined& event);
    void onGameStarted(const Event::GameStarted& event);
    void onHoleCardsDealt(const Event::HoleCardsDealt& event);
    void onBlindPosted(const Event::BlindPosted& event);
    void onCommunityCardsDealt(const Event::CommunityCardsDealt& event);
    void onBettingRoundStarted(const Event::BettingRoundStarted& event);
    void onBettingRoundEnded(const Event::BettingRoundEnded& event);
    void onAwaitPlayerInput(const Event::AwaitPlayerInput& event);
    void onPlayerActed(const Event::PlayerActed& event);
    void onHoleCardsRevealed(const Event::HoleCardsRevealed& event);
};
} // namespace poker