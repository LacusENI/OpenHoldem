#include "BettingRound.h"

namespace poker {
BettingRound::BettingRound() {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundFinished() const {
    return algo::isRoundFinished(round_state);
}

bool BettingRound::isOnlyOnePlayerActive() const {
    return algo::isOnlyOnePlayerActive(round_state);
}

void BettingRound::onGameStarted(int player_count) {
    player_bets.resize(player_count, 0);
    algo::setupGameStart(round_state, player_count);
}

void BettingRound::onRoundStarted(seat_t start_position) {
    actor_seat = start_position;
    round_bet = 0;
    min_raise_delta = 0;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    algo::setupRoundStart(round_state);
}

algo::BettingContext BettingRound::getBettingContext(chips_t actor_chips) const {
    return {
        .player_chips = actor_chips,
        .player_bet = getPlayerBetAtSeat(actor_seat),
        .round_bet = getRoundBet(),
        .min_raise_delta = getMinRaiseDelta(),
    };
}

void BettingRound::applyBettingResult(const algo::BettingResult& result) {
    player_bets[getActorSeat()] = result.player_bet;
    round_bet = result.round_bet;
    min_raise_delta = result.min_raise_delta;
    algo::handlePlayerAction(round_state, result.action);
}

void BettingRound::processForcedBlind(seat_t sb_seat, seat_t bb_seat, chips_t big_blind) {
    player_bets[sb_seat] = big_blind / 2;
    player_bets[bb_seat] = big_blind;
    round_bet = big_blind;
    min_raise_delta = big_blind;
}

void BettingRound::advanceToNextActor(seat_t next_actor_seat) {
    actor_seat = next_actor_seat;
}
} // namespace poker