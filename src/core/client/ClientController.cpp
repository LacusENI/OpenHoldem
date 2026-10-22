#include "ClientController.h"

#include <algorithm>
#include <cstddef>

#include "ClientModel.h"
#include "ClientView.h"
#include "IServer.h"

namespace poker {

void ClientController::onJoinResponse(const Event::JoinResponse& event) {
    model.this_player_seat = event.seat;
}

void ClientController::onPlayerJoined(const Event::PlayerJoined&) {}

void ClientController::onGameStarted(const Event::GameStarted& event) {
    model.btn_seat = event.btn_seat;
    model.big_blind = event.big_blind;
    model.players.resize(event.player_count);
    model.player_states.resize(event.player_count);
    model.player_bets.resize(event.player_count);
    view.render(model);
}

void ClientController::onHoleCardsDealt(const Event::HoleCardsDealt& event) {
    if (event.player_seat == model.this_player_seat) {
        model.players[event.player_seat].setHoleCards(event.hole_cards);
        view.render(model);
    }
}

void ClientController::onStageChanged(const Event::StageChanged&) {
    for (auto& bet : model.player_bets) bet = 0;
    for (auto& state : model.player_states) {
        if (state != PlayerStatus::FOLDED && state != PlayerStatus::ALL_IN) {
            state = PlayerStatus::IDLE;
        }
    }
    view.render(model);
}

void ClientController::onBlindPosted(const Event::BlindPosted& event) {
    model.player_states[event.player] =
        event.is_big_blind ? PlayerStatus::BIG_BLIND : PlayerStatus::SMALL_BLIND;
    model.pot += event.amount;
    model.players[event.player].payChips(event.amount);
    model.player_bets[event.player] = event.amount;
    view.render(model);
}

void ClientController::onCommunityCardsDealt(const Event::CommunityCardsDealt& event) {
    model.community_cards = event.community_cards;
    view.render(model);
}

void ClientController::onTurnStarted(const Event::TurnStarted& event) {
    if (event.player != model.this_player_seat) return;
    // 默认策略：能过牌就过牌，否则跟注（筹码不足即 all-in 跟注）。
    const chips_t target_bet =
        (event.round_bet <= event.player_bet)
            ? event.player_bet
            : std::min(event.round_bet, event.player_bet + event.player_chips);
    server.onSubmit(Command::SubmitAction{.target_bet = target_bet, .is_fold = false});
}

void ClientController::onPlayerActed(const Event::PlayerActed& event) {
    PlayerStatus state;
    switch (event.action) {
    case Action::Call: state = PlayerStatus::CALLED; break;
    case Action::Check: state = PlayerStatus::CHECKED; break;
    case Action::Raise: state = PlayerStatus::RAISED; break;
    case Action::Bet: state = PlayerStatus::BET; break;
    case Action::Fold: state = PlayerStatus::FOLDED; break;
    default: state = PlayerStatus::IDLE; break;
    }
    // all-in 由玩家状态表达（Action 不再区分完整 all-in）
    if (event.is_all_in) state = PlayerStatus::ALL_IN;
    model.player_states[event.player] = state;

    const chips_t delta = event.bet - model.player_bets[event.player];
    model.pot += delta;
    model.players[event.player].payChips(delta);
    model.player_bets[event.player] = event.bet;
    view.render(model);
}

void ClientController::onHoleCardsRevealed(const Event::HoleCardsRevealed&) {}

void ClientController::onPotAwarded(const Event::PotAwarded& event) {
    if (!event.award_to.empty()) {
        const auto winners = static_cast<chips_t>(event.award_to.size());
        const chips_t share = event.chips / winners;
        const chips_t remainder = event.chips % winners;
        for (std::size_t i = 0; i < event.award_to.size(); ++i) {
            const chips_t extra = static_cast<chips_t>(i) < remainder ? 1 : 0;
            model.players[event.award_to[i]].winChips(share + extra);
        }
    }
    model.pot = 0;
    view.render(model);
}

void ClientController::onUncalledBetReturned(const Event::UncalledBetReturned& event) {
    model.players[event.player].winChips(event.amount);
    model.pot = std::max<chips_t>(0, model.pot - event.amount);
    view.render(model);
}

void ClientController::onGameEnded(const Event::GameEnded&) { view.render(model); }

} // namespace poker
