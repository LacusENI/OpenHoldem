#include "BettingRound.h"
#include "PotManager.h"
#include "PlayerSet.h"
#include "RoundTracker.h"

namespace holdem {
constexpr Stack BIG_BLIND = 10;

Action BettingRound::processAction(const Action& action) const {
    if (action.type == ActionType::FOLD)
        return {action.actor_position, ActionType::FOLD, action.amount};
    return {action.actor_position, ActionType::BET, action.amount};
}

BettingRound::BettingRound(
    std::shared_ptr<PlayerSet> players,
    std::shared_ptr<PotManager> pot_manager
    ) :
    player_set(std::move(players)),
    pot_manager(std::move(pot_manager)),
    round_tracker(std::make_unique<RoundTracker>()) {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundEnded() const {
    return round_tracker->isRoundEnded();
}

void BettingRound::onGameStarted() {
    const int players_count = player_set->size();
    player_bets.resize(players_count, 0);
    round_tracker->onGameStarted(players_count);
}

void BettingRound::onRoundStarted(Position start_position) {
    current_position = start_position;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_bet = 0;
    round_tracker->onRoundStarted();
}

void BettingRound::nextTurn() {
    current_position = player_set->nextPositionToAct(current_position);
}

void BettingRound::commitChips(Position position, Stack amount) {
    Player& player = player_set->at(position);
    Stack player_bet = player_bets[position];
    player.chips -= amount;
    player_bet += amount;
    if (player_bet > round_bet) {
        round_bet = player_bet;
    }
    player_bets[position] = player_bet;
    pot_manager->addChipsToPot(amount);
}

Action BettingRound::handleAction(const Action& action) {
    auto [position, action_type, amount] = processAction(action);
    commitChips(position, amount);
    round_tracker->onPlayerActed(action_type);
    nextTurn();
    return {position, action_type, amount};
}


} // namespace holdem