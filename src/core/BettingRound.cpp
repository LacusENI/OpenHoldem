#include "BettingRound.h"
#include "PotManager.h"
#include "RoundTracker.h"
#include "SeatingManager.h"

namespace poker {
BettingRound::BettingRound(
    std::shared_ptr<SeatingManager> seating_manager,
    std::shared_ptr<PotManager> pot_manager
    ) :
    seating_manager(std::move(seating_manager)),
    pot_manager(std::move(pot_manager)),
    round_tracker(std::make_unique<RoundTracker>()) {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundEnded() const {
    return round_tracker->isRoundEnded();
}

bool BettingRound::isOnlyOnePlayerActive() const {
    return round_tracker->isOnlyOnePlayerActive();
}

bool BettingRound::isNoPlayerNeedToAct() const {
    return round_tracker->isNoPlayerNeedToAct();
}

void BettingRound::onGameStarted() {
    const int player_count = seating_manager->getPlayerCount();
    player_bets.resize(player_count, 0);
    round_tracker->onGameStarted(player_count);
}

void BettingRound::startBettingRound(seat_idx_t start_position) {
    actor_seat = start_position;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_bet = 0;
    round_tracker->onRoundStarted();
}

math::BettingContext BettingRound::getBettingContext() const {
    return {
        .player_chips = seating_manager->getPlayerAtSeat(actor_seat).chips,
        .player_bet = getPlayerBetAt(actor_seat),
        .round_bet =  getRoundBet(),
        .min_raise_delta = getMinRaiseDelta(),
    };
}

void BettingRound::processBettingInput(math::BettingInput betting_input) {
    math::BettingContext betting_context = getBettingContext();
    math::BettingResult result = math::process_betting(betting_context, betting_input);
    Player& actor = seating_manager->getPlayerAtSeat(actor_seat);
    actor.bet(result.bet_delta);
    pot_manager->addChipsToPot(result.bet_delta);
    player_bets[getActorSeat()] = result.player_bet;
    round_bet = result.round_bet;
    min_raise_delta = result.min_raise_delta;
}

void BettingRound::advanceToNextActor() {
    actor_seat = seating_manager->nextPositionToAct(actor_seat);
}
} // namespace holdem