#include "ClientController.h"

#include "ClientModel.h"
#include "ClientView.h"
#include "EventBus.hpp"

namespace poker {
ClientController::ClientController(std::shared_ptr<EventBus> event_bus,
                                   std::unique_ptr<ClientModel> client_model,
                                   std::unique_ptr<ClientView> client_view)
    : event_bus(std::move(event_bus)),
      client_model(std::move(client_model)),
      client_view(std::move(client_view)) {}
ClientController::~ClientController() = default;
void ClientController::subscribeEvent() {
    event_bus->subscribe<Event::PlayerJoinResponse>(
        [this](const auto& event) { this->onPlayerJoinSucceeded(event); });
    event_bus->subscribe<Event::GameStarted>(
        [this](const auto& event) { this->onGameStarted(event); });
    event_bus->subscribe<Event::HoleCardsDealt>(
        [this](const auto& event) { this->onHoleCardsDealt(event); });
    event_bus->subscribe<Event::BlindPosted>(
        [this](const auto& event) { this->onBlindPosted(event); });
    event_bus->subscribe<Event::CommunityCardsDealt>(
        [this](const auto& event) { this->onCommunityCardsDealt(event); });
    event_bus->subscribe<Event::BettingRoundStarted>(
        [this](const auto& event) { this->onBettingRoundStarted(event); });
    event_bus->subscribe<Event::BettingRoundEnded>(
        [this](const auto& event) { this->onBettingRoundEnded(event); });
    event_bus->subscribe<Event::PlayerActed>(
        [this](const auto& event) { this->onPlayerActed(event); });
    event_bus->subscribe<Event::AwaitPlayerAction>(
        [this](const auto& event) { this->onAwaitPlayerInput(event); });
}
void ClientController::joinGame() {
    event_bus->publish(Event::PlayerJoinRequest{});
}
void ClientController::onPlayerJoinSucceeded(const Event::PlayerJoinResponse& event) {
    client_model->this_player_seat = event.player_seat;
}
void ClientController::onGameJoined(const Event::NewPlayerJoined& event) {}
void ClientController::onGameStarted(const Event::GameStarted& event) {
    client_model->btn_seat = event.btn_seat;
    client_model->big_blind = event.big_blind;
    client_model->players.resize(event.player_count);
    for (seat_idx_t seat = 0; seat < client_model->players.size(); ++seat) {
        client_model->players[seat].position = seat;
    }
    client_model->player_states.resize(event.player_count);
    client_model->player_bets.resize(event.player_count);
    client_view->render(*client_model);
}
void ClientController::onHoleCardsDealt(const Event::HoleCardsDealt& event) {
    if (event.player_seat == client_model->this_player_seat) {
        client_model->players[event.player_seat].hole_cards = event.hole_cards;
        client_view->render(*client_model);
    }
}
void ClientController::onBlindPosted(const Event::BlindPosted& event) {
    if (event.is_big_blind) {
        client_model->player_states[event.seat] = PlayerState::BIG_BLIND;
    } else {
        client_model->player_states[event.seat] = PlayerState::SMALL_BLIND;
    }
    client_model->pot = event.pot;
    client_model->players[event.seat].chips = event.player_chips;
    client_model->player_bets[event.seat] = event.player_bet;
    client_view->render(*client_model);
}
void ClientController::onCommunityCardsDealt(const Event::CommunityCardsDealt& event) {
    client_model->community_cards = event.community_cards;
    client_view->render(*client_model);
}
void ClientController::onBettingRoundStarted(const Event::BettingRoundStarted& event) {}
void ClientController::onBettingRoundEnded(const Event::BettingRoundEnded& event) {
    for (int i = 0; i < client_model->player_bets.size(); ++i) {
        client_model->player_bets[i] = 0;
    }
    for (int i = 0; i < client_model->player_states.size(); ++i) {
        client_model->player_states[i] = PlayerState::IDLE;
    }
    client_view->render(*client_model);
}

void ClientController::onAwaitPlayerInput(const Event::AwaitPlayerAction& event) {
    if (event.player_seat == client_model->this_player_seat) {
        event_bus->publish(
            Event::PlayerActionSubmitted{.target_bet = client_model->big_blind, .is_fold = false});
    }
}

void ClientController::onPlayerActed(const Event::PlayerActed& event) {
    PlayerState player_state;
    switch (event.action) {
    case Action::CALL: player_state = PlayerState::CALLED; break;
    case Action::CHECK: player_state = PlayerState::CHECKED; break;
    case Action::RAISE: player_state = PlayerState::RAISED; break;
    case Action::BET: player_state = PlayerState::BET; break;
    case Action::FOLD: player_state = PlayerState::FOLDED; break;
    case Action::ALL_IN: player_state = PlayerState::ALL_IN; break;
    default: player_state = PlayerState::IDLE;
    }
    client_model->player_states[event.actor_seat] = player_state;
    client_model->pot = event.pot;
    client_model->players[event.actor_seat].chips = event.player_chips;
    client_model->player_bets[event.actor_seat] = event.player_bet;
    client_view->render(*client_model);
}
void ClientController::onHoleCardsRevealed(const Event::HoleCardsRevealed& event) {}

} // namespace poker