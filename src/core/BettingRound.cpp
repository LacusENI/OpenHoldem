#include "BettingRound.h"
#include "PotManager.h"
#include "PlayerSet.h"

namespace holdem {
constexpr Stack BIG_BLIND = 10;

Action BettingRound::processAction(const Action& action) const {
    Player& player = player_set->at(current_position);
    if (action.type == ActionType::FOLD) {
        player.is_folded = true;
        return {current_position, ActionType::FOLD, 0};
    }
    ActionType action_type;
    Stack amount = 0;
    Stack player_bet = player_bets[current_position];
    if (round_bet == 0) {
        action_type = ActionType::BET;
        amount = BIG_BLIND;
    } else if (player_bet < round_bet) {
        action_type = ActionType::CALL;
        amount = round_bet - player_bet;
    } else {
        action_type = ActionType::CHECK;
    }
    return {action.actor_position, action_type, amount};
}

void BettingRound::updateState(Position position, bool is_raise, bool is_fold) {
    if (is_raise) {
        pending_players_count = players_can_act_count;
        last_raiser_position = position;
    } else {
        pending_players_count--;
    }
    if (is_fold) active_players_count--;
    if (is_fold) players_can_act_count--;
}

BettingRound::BettingRound(
    std::shared_ptr<PlayerSet> players,
    std::shared_ptr<PotManager> pot_manager
    )
    : player_set(std::move(players)), pot_manager(std::move(pot_manager)) {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundEnded() const {
    return (active_players_count == 1 || players_can_act_count == 1 || pending_players_count == 0);
}

void BettingRound::setup() {
    const int players_count = player_set->size();
    player_bets.resize(players_count, 0);
    active_players_count = players_count;
    players_can_act_count = players_count;
}

void BettingRound::prepare(Position start_position) {
    current_position = start_position;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_bet = 0;
    pending_players_count = players_can_act_count;
    last_raiser_position = -1;
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
    bool is_raise = action_type == ActionType::RAISE;
    bool is_fold = action_type == ActionType::FOLD;
    commitChips(position, amount);
    updateState(position, is_raise, is_fold);
    nextTurn();
    return {position, action_type, amount};
}


} // namespace holdem