#include "BettingRound.h"
#include "RoundTracker.h"

namespace poker {
BettingRound::BettingRound(
    ) : round_tracker(std::make_unique<RoundTracker>()) {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundFinished() const {
    return round_tracker->isRoundFinished();
}

bool BettingRound::isOnlyOnePlayerActive() const {
    return round_tracker->isOnlyOnePlayerActive();
}

void BettingRound::onGameStarted(int player_count) {
    player_bets.resize(player_count, 0);
    round_tracker->onGameStarted(player_count);
}

void BettingRound::onRoundStarted(seat_idx_t start_position) {
    actor_seat = start_position;
    round_bet = 0;
    min_raise_delta = 0;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_tracker->onRoundStarted();
}

math::BettingContext BettingRound::getBettingContext(chips_t actor_chips) const {
    return {
        .player_chips = actor_chips,
        .player_bet = getPlayerBetAtSeat(actor_seat),
        .round_bet =  getRoundBet(),
        .min_raise_delta = getMinRaiseDelta(),
    };
}

void BettingRound::applyBettingResult(const math::BettingResult& result) {
    player_bets[getActorSeat()] = result.player_bet;
    round_bet = result.round_bet;
    min_raise_delta = result.min_raise_delta;
    round_tracker->onPlayerActed(result.action_type);
}

void BettingRound::processForcedBlind(seat_idx_t sb_seat, seat_idx_t bb_seat, chips_t big_blind) {
    player_bets[sb_seat] = big_blind / 2;
    player_bets[bb_seat] = big_blind;
    round_bet = big_blind;
    min_raise_delta = big_blind;
}

void BettingRound::advanceToNextActor(seat_idx_t next_actor_seat) {
    actor_seat = next_actor_seat;
}
} // namespace poker