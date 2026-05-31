#include "BettingRound.h"
#include "PotManager.h"
#include "RoundTracker.h"
#include "SeatingManager.h"

namespace poker {
constexpr chips_t BIG_BLIND = 10;

Action BettingRound::processAction(const Action& action) const {
    if (action.type == ActionType::FOLD)
        return {action.actor_position, ActionType::FOLD, action.amount};
    if (action.amount == getChipsToCall())
        return {action.actor_position, ActionType::CALL, action.amount};
    return {action.actor_position, ActionType::RAISE, action.amount};
}

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

void BettingRound::onGameStarted() {
    const int player_count = seating_manager->getPlayerCount();
    player_bets.resize(player_count, 0);
    round_tracker->onGameStarted(player_count);
}

void BettingRound::onRoundStarted(seat_idx_t start_position) {
    current_position = start_position;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_bet = 0;
    round_tracker->onRoundStarted();
}

void BettingRound::nextTurn() {
    current_position = seating_manager->nextPositionToAct(current_position);
}

void BettingRound::commitChips(seat_idx_t position, chips_t amount) {
    Player& player = seating_manager->getPlayerAtSeat(position);
    chips_t player_bet = player_bets[position];
    player.chips -= amount;
    player_bet += amount;
    if (player_bet > round_bet) {
        min_raise = player_bet - round_bet;
        round_bet = player_bet;
    }
    player_bets[position] = player_bet;
    pot_manager->addChipsToPot(amount);
}

chips_t BettingRound::getChipsToCall() const {
    chips_t player_bet = player_bets[current_position];
    return round_bet - player_bet;
}

chips_t BettingRound::getChipsToMinRaise() const {
    chips_t player_bet = player_bets[current_position];
    return round_bet + min_raise - player_bet;
}

Action BettingRound::handleAction(const Action& action) {
    auto [position, action_type, amount] = processAction(action);
    commitChips(position, amount);
    round_tracker->onPlayerActed(action_type);
    nextTurn();
    return {position, action_type, amount};
}


} // namespace holdem