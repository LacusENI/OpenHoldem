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
    event_bus->subscribe<Event::WaitingForAction>(&ClientController::handle, this);
    event_bus->subscribe<Event::PlayerActed>(&ClientController::handle, this);
    event_bus->subscribe<Event::HoleCardsRevealed>(&ClientController::handle, this);
}
void ClientController::handle(const Event::PlayerJoinResponse& event) {
    GlobalLogger::debug("[ClientController] EventListener(PlayerJoinResponse)");
    client_model->this_player_seat = event.player_seat;
}
void ClientController::handle(const Event::NewPlayerJoined& _) {
    GlobalLogger::debug("[ClientController] EventListener(NewPlayerJoined)");
}
void ClientController::handle(const Event::GameStarted& event) {
    GlobalLogger::debug("[ClientController] EventListener(GameStarted)");
    client_model->btn_seat = event.btn_seat;
    client_model->big_blind = event.big_blind;
    client_model->players.resize(event.player_count);
    client_model->player_states.resize(event.player_count);
    client_model->player_bets.resize(event.player_count);
    client_view->render(*client_model);
}
void ClientController::handle(const Event::HoleCardsDealt& event) {
    GlobalLogger::debug("[ClientController] EventListener(HoleCardsDealt)");
    if (event.player_seat == client_model->this_player_seat) {
        client_model->players[event.player_seat].setHoleCards(event.hole_cards);
        client_view->render(*client_model);
    }
}
void ClientController::handle(const Event::BlindPosted& event) {
    GlobalLogger::debug("[ClientController] EventListener(BlindPosted)");
    if (event.is_big_blind) {
        client_model->player_states[event.player] = PlayerStatus::BIG_BLIND;
    } else {
        client_model->player_states[event.player] = PlayerStatus::SMALL_BLIND;
    }
    client_model->pot += event.amount;
    client_model->players[event.player].commitChips(event.amount);
    client_model->player_bets[event.player] = event.amount;
    client_view->render(*client_model);
}
void ClientController::handle(const Event::CommunityCardsDealt& event) {
    GlobalLogger::debug("[ClientController] EventListener(CommunityCardsDealt)");
    client_model->community_cards = event.community_cards;
    client_view->render(*client_model);
}
void ClientController::handle(const Event::BettingRoundStarted& _) {
    GlobalLogger::debug("[ClientController] EventListener(BettingRoundStarted)");
}
void ClientController::handle(const Event::BettingRoundEnded& _) {
    GlobalLogger::debug("[ClientController] EventListener(BettingRoundEnded)");
    for (size_t i = 0; i < client_model->player_bets.size(); ++i) {
        client_model->player_bets[i] = 0;
    }
    for (size_t i = 0; i < client_model->player_states.size(); ++i) {
        client_model->player_states[i] = PlayerStatus::IDLE;
    }
    client_view->render(*client_model);
}

void ClientController::handle(const Event::WaitingForAction& event) {
    GlobalLogger::debug("[ClientController] EventListener(AwaitPlayerAction)");
    if (event.player == client_model->this_player_seat) {
        event_bus->publish(Event::PlayerAction{
            .target_bet = client_model->big_blind, .is_fold = false, .is_all_in = false});
    }
}

void ClientController::handle(const Event::PlayerActed& event) {
    /**/
    GlobalLogger::debug("[ClientController] EventListener(PlayerActed)");
    PlayerStatus player_state;
    switch (event.action) {
    case Action::CALL: player_state = PlayerStatus::CALLED; break;
    case Action::CHECK: player_state = PlayerStatus::CHECKED; break;
    case Action::RAISE: player_state = PlayerStatus::RAISED; break;
    case Action::BET: player_state = PlayerStatus::BET; break;
    case Action::FOLD: player_state = PlayerStatus::FOLDED; break;
    case Action::ALL_IN_UNDER_CALL:
    case Action::ALL_IN_UNDER_RAISE:
    case Action::ALL_IN_CALL:
    case Action::ALL_IN_RAISE: player_state = PlayerStatus::ALL_IN; break;
    default: player_state = PlayerStatus::IDLE;
    }
    client_model->player_states[event.player] = player_state;
    chips_t delta = event.bet - client_model->player_bets[event.player];
    client_model->pot += delta;
    client_model->players[event.player].commitChips(delta);
    client_model->player_bets[event.player] = event.bet;
    client_view->render(*client_model);
}
void ClientController::handle(const Event::HoleCardsRevealed& _) {
    GlobalLogger::debug("[ClientController] EventListener(HoleCardsRevealed)");
}

} // namespace poker