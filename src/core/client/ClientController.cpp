#include "ClientController.h"

#include "ClientModel.h"
#include "ClientView.h"
#include "EventBus.hpp"
#include "GlobalLogger.h"

namespace poker {

ClientController::ClientController(EventBus* event_bus, ClientModel* client_model,
                                   ClientView* client_view)
    : event_bus(event_bus), client_model(client_model), client_view(client_view) {}

void ClientController::subscribeEvent() {
    event_bus->subscribe<Event::PlayerJoinResponse>(&ClientController::handle, this);
    event_bus->subscribe<Event::GameStarted>(&ClientController::handle, this);
    event_bus->subscribe<Event::HoleCardsDealt>(&ClientController::handle, this);
    event_bus->subscribe<Event::BlindPosted>(&ClientController::handle, this);
    event_bus->subscribe<Event::CommunityCardsDealt>(&ClientController::handle, this);
    event_bus->subscribe<Event::BettingRoundStarted>(&ClientController::handle, this);
    event_bus->subscribe<Event::BettingRoundEnded>(&ClientController::handle, this);
    event_bus->subscribe<Event::AwaitPlayerAction>(&ClientController::handle, this);
    event_bus->subscribe<Event::PlayerActed>(&ClientController::handle, this);
    event_bus->subscribe<Event::HoleCardsRevealed>(&ClientController::handle, this);
}
void ClientController::handle(const Event::PlayerJoinResponse& event) {
    GlobalLogger::debug("[ClientController] EventListener(PlayerJoinResponse)");
    client_model->this_player_seat = event.player_seat;
}
void ClientController::handle(const Event::NewPlayerJoined& event) {
    GlobalLogger::debug("[ClientController] EventListener(NewPlayerJoined)");
}
void ClientController::handle(const Event::GameStarted& event) {
    GlobalLogger::debug("[ClientController] EventListener(GameStarted)");
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
void ClientController::handle(const Event::HoleCardsDealt& event) {
    GlobalLogger::debug("[ClientController] EventListener(HoleCardsDealt)");
    if (event.player_seat == client_model->this_player_seat) {
        client_model->players[event.player_seat].hole_cards = event.hole_cards;
        client_view->render(*client_model);
    }
}
void ClientController::handle(const Event::BlindPosted& event) {
    GlobalLogger::debug("[ClientController] EventListener(BlindPosted)");
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
void ClientController::handle(const Event::CommunityCardsDealt& event) {
    GlobalLogger::debug("[ClientController] EventListener(CommunityCardsDealt)");
    client_model->community_cards = event.community_cards;
    client_view->render(*client_model);
}
void ClientController::handle(const Event::BettingRoundStarted& event) {
    GlobalLogger::debug("[ClientController] EventListener(BettingRoundStarted)");
}
void ClientController::handle(const Event::BettingRoundEnded& event) {
    GlobalLogger::debug("[ClientController] EventListener(BettingRoundEnded)");
    for (int i = 0; i < client_model->player_bets.size(); ++i) {
        client_model->player_bets[i] = 0;
    }
    for (int i = 0; i < client_model->player_states.size(); ++i) {
        client_model->player_states[i] = PlayerState::IDLE;
    }
    client_view->render(*client_model);
}

void ClientController::handle(const Event::AwaitPlayerAction& event) {
    GlobalLogger::debug("[ClientController] EventListener(AwaitPlayerAction)");
    if (event.player_seat == client_model->this_player_seat) {
        event_bus->publish(
            Event::PlayerActionSubmitted{.target_bet = client_model->big_blind, .is_fold = false});
    }
}

void ClientController::handle(const Event::PlayerActed& event) {
    GlobalLogger::debug("[ClientController] EventListener(PlayerActed)");
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
void ClientController::handle(const Event::HoleCardsRevealed& event) {
    GlobalLogger::debug("[ClientController] EventListener(HoleCardsRevealed)");
}

} // namespace poker