#pragma once
#include <memory>

#include "Event.h"

namespace poker {
class ClientModel;
class ClientView;

class ClientController {
private:
    std::unique_ptr<ClientModel> client_model;
    std::unique_ptr<ClientView> client_view;
public:
    ClientController(std::unique_ptr<ClientModel> client_model, std::unique_ptr<ClientView> client_view);
    ~ClientController();

    void onHoleCardsDealt(const Event::HoleCardsDealt& event);
    void onStageChanged(const Event::StageChanged& event);
    void onBlindPosted(const Event::BlindPosted& event);
    void onCommunityCardsDealt(const Event::CommunityCardsDealt& event);
    void onBettingRoundStarted(const Event::BettingRoundStarted& event);
    void onBettingRoundEnded(const Event::BettingRoundEnded& event);
    void onAwaitPlayerInput(const Event::AwaitPlayerInput& event);
    void onPlayerInputSubmitted(const Event::PlayerInputSubmitted& event);
    void onPlayerActed(const Event::PlayerActed& event);
    void onHoleCardsRevealed(const Event::HoleCardsRevealed& event);
};
} // namespace poker