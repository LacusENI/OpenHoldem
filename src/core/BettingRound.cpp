#include "BettingRound.h"
#include "PotManager.h"
#include "PlayerSet.h"

namespace holdem {

BettingRound::BettingRound(
    std::shared_ptr<PlayerSet> players,
    std::shared_ptr<PotManager> pot_manager
    )
    : players(std::move(players)), pot_manager(std::move(pot_manager)) {}

BettingRound::~BettingRound() = default;

bool BettingRound::isRoundEnded() const {
    return is_round_ended || players->nextPositionToAct(current_position) == current_position;
}

void BettingRound::prepare(Position start_position) {
    player_bets.resize(players->size(), 0);
    current_position = start_position;
    rest_position = start_position;
    is_round_ended = false;
    for (auto& bet : player_bets) {
        bet = 0;
    }
    round_bet = 0;
}

void BettingRound::nextTurn() {
    Position next_position = players->nextPositionToAct(current_position);
    is_round_ended =
        next_position == rest_position ||
            players->nextPositionToAct(next_position) == next_position;
    if (players->at(current_position).is_folded
        && current_position == rest_position) {
        rest_position = next_position;
        }
    current_position = next_position;
}

void BettingRound::commitChips(Position position, Stack amount) {
    Player& player = players->at(position);
    Stack player_bet = player_bets[position];
    player.chips -= amount;
    player_bet += amount;
    if (player_bet > round_bet) {
        round_bet = player_bet;
    }
    player_bets[position] = player_bet;
    pot_manager->addChipsToPot(amount);
}

Action BettingRound::handleAction(Action action, Stack big_blind) {
    Player& player = players->at(current_position);
    if (action.type == ActionType::FOLD) {
        player.is_folded = true;
        return {current_position, ActionType::FOLD, 0};
    }
    ActionType action_type;
    Stack amount = 0;
    Stack player_bet = player_bets[current_position];
    if (round_bet == 0) {
        action_type = ActionType::BET;
        amount = big_blind;
    } else if (player_bet < round_bet) {
        action_type = ActionType::CALL;
        amount = round_bet - player_bet;
    } else {
        action_type = ActionType::CHECK;
    }
    commitChips(action.actor_position, amount);
    return {current_position, action_type, amount};
}


} // namespace holdem