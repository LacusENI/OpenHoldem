#include "chips_calc.h"

#include <algorithm>

namespace poker::algo {
BettingResult process_betting(BettingContext context, BettingInput input) {
    BettingResult result;
    result.bet_delta = input.target_bet - context.player_bet;
    result.player_bet = input.target_bet;
    result.round_bet = std::max(context.round_bet, input.target_bet);
    result.min_raise_delta = std::max(context.min_raise_delta, input.target_bet - context.round_bet);
    result.action_type = ActionType::VOID;
    result.is_target_bet_valid = true;
    result.has_sufficient_chips = result.bet_delta <= context.player_chips;
    if (input.is_fold) {
        result.action_type = ActionType::FOLD;
    }
    else if (input.target_bet < context.player_bet) {
        result.is_target_bet_valid = false;
    } else if (input.target_bet == context.round_bet) {
        if (input.target_bet == context.player_bet) {
            result.action_type = ActionType::CHECK;
        } else {
            result.action_type = ActionType::CALL;
        }
    } else if (input.target_bet > context.round_bet + context.min_raise_delta) {
        if (context.round_bet == 0) {
            result.action_type = ActionType::BET;
        } else {
            result.action_type = ActionType::RAISE;
        }
    } else if (result.bet_delta == context.player_chips) {
        result.action_type = ActionType::ALL_IN;
    } else {
        result.is_target_bet_valid = false;
    }
    return result;
}

} // namespace poker::math