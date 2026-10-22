#include "ClientController.h"

#include <algorithm>
#include <cstddef>

#include "ClientModel.h"
#include "ClientView.h"
#include "Event.hpp"
#include "EventBus.hpp"

namespace poker {

void ClientController::subscribeEvent() {
    event_bus.subscribe<Event::JoinResponse>(&ClientController::handle, this);
    event_bus.subscribe<Event::GameStarted>(&ClientController::handle, this);
    event_bus.subscribe<Event::HoleCardsDealt>(&ClientController::handle, this);
    event_bus.subscribe<Event::BlindPosted>(&ClientController::handle, this);
    event_bus.subscribe<Event::CommunityCardsDealt>(&ClientController::handle, this);
    event_bus.subscribe<Event::TurnStarted>(&ClientController::handle, this);
    event_bus.subscribe<Event::PlayerActed>(&ClientController::handle, this);
    event_bus.subscribe<Event::HoleCardsRevealed>(&ClientController::handle, this);
}

void ClientController::handle(const Event::JoinResponse& event) {
    client_model.this_player_seat = event.seat;
}

void ClientController::handle(const Event::PlayerJoined& _) {}

void ClientController::handle(const Event::GameStarted& event) {
    client_model.btn_seat = event.btn_seat;
    client_model.big_blind = event.big_blind;
    client_model.players.resize(event.player_count);
    client_model.player_states.resize(event.player_count);
    client_model.player_bets.resize(event.player_count);
    client_view.render(client_model);
}

void ClientController::handle(const Event::HoleCardsDealt& event) {
    if (event.player_seat == client_model.this_player_seat) {
        client_model.players[event.player_seat].setHoleCards(event.hole_cards);
        client_view.render(client_model);
    }
}

void ClientController::handle(const Event::BlindPosted& event) {
    if (event.is_big_blind) {
        client_model.player_states[event.player] = PlayerStatus::BIG_BLIND;
    } else {
        client_model.player_states[event.player] = PlayerStatus::SMALL_BLIND;
    }
    client_model.pot += event.amount;
    client_model.players[event.player].payChips(event.amount);
    client_model.player_bets[event.player] = event.amount;
    client_view.render(client_model);
}

void ClientController::handle(const Event::CommunityCardsDealt& event) {
    client_model.community_cards = event.community_cards;
    client_view.render(client_model);
}

void ClientController::handle(const int _ /* TODO */) {
    for (std::size_t i = 0; i < client_model.player_bets.size(); ++i) {
        client_model.player_bets[i] = 0;
    }
    for (std::size_t i = 0; i < client_model.player_states.size(); ++i) {
        client_model.player_states[i] = PlayerStatus::IDLE;
    }
    client_view.render(client_model);
}

void ClientController::handle(const Event::TurnStarted& event) {
    if (event.player != client_model.this_player_seat) return;
    // 默认策略：能过牌就过牌，否则跟注（筹码不足即 all-in 跟注）。
    const chips_t target_bet =
        (event.round_bet <= event.player_bet)
            ? event.player_bet
            : std::min(event.round_bet, event.player_bet + event.player_chips);
    server_gateway.send(Command::SubmitAction{.target_bet = target_bet, .is_fold = false});
}

void ClientController::handle(const Event::PlayerActed& event) {
    PlayerStatus player_state;
    switch (event.action) {
    case Action::Call: player_state = PlayerStatus::CALLED; break;
    case Action::Check: player_state = PlayerStatus::CHECKED; break;
    case Action::Raise: player_state = PlayerStatus::RAISED; break;
    case Action::Bet: player_state = PlayerStatus::BET; break;
    case Action::Fold: player_state = PlayerStatus::FOLDED; break;
    default: player_state = PlayerStatus::IDLE; break;
    }
    // all-in 由玩家状态表达（Action 不再区分完整 all-in）
    if (event.is_all_in) player_state = PlayerStatus::ALL_IN;
    client_model.player_states[event.player] = player_state;
    chips_t delta = event.bet - client_model.player_bets[event.player];
    client_model.pot += delta;
    client_model.players[event.player].payChips(delta);
    client_model.player_bets[event.player] = event.bet;
    client_view.render(client_model);
}

void ClientController::handle(const Event::HoleCardsRevealed& _) {}

} // namespace poker
