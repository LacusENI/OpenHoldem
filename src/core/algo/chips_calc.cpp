#include "chips_calc.h"

#include <algorithm>

#include "GameTypes.hpp"

namespace poker::algo {
BettingResult process_betting(BettingContext context, BettingInput input) {
    BettingResult result;
    result.bet_delta = input.target_bet - context.player_bet;
    result.player_bet = input.target_bet;
    result.round_bet = std::max(context.round_bet, input.target_bet);
    chips_t raise_delta = input.target_bet - context.round_bet;
    result.min_raise_delta = std::max(context.min_raise_delta, raise_delta);
    result.action = Action::VOID;
    result.is_target_bet_valid = true;
    result.has_sufficient_chips = result.bet_delta <= context.player_chips;
    if (input.is_fold) {
        result.action = Action::FOLD;
    } else if (result.bet_delta == context.player_chips) {
        if (input.target_bet < context.round_bet) {
            result.action = Action::ALL_IN_UNDER_CALL;
        } else if (input.target_bet == context.round_bet) {
            result.action = Action::ALL_IN_CALL;
        } else if (input.target_bet < context.min_raise_delta + context.round_bet) {
            result.action = Action::ALL_IN_UNDER_RAISE;
        } else {
            result.action = Action::ALL_IN_RAISE;
        }
    } else if (input.target_bet < context.player_bet) {
        result.is_target_bet_valid = false;
    } else if (input.target_bet == context.round_bet) {
        if (input.target_bet == context.player_bet) {
            result.action = Action::CHECK;
        } else {
            result.action = Action::CALL;
        }
    } else if (input.target_bet >= context.round_bet + context.min_raise_delta) {
        if (context.round_bet == 0) {
            result.action = Action::BET;
        } else {
            result.action = Action::RAISE;
        }
    } else {
        result.is_target_bet_valid = false;
    }
    return result;
}

std::vector<chips_t> calculate_awards(int pot, int winners_n) {
    const int share = pot / winners_n;
    const int remainder = pot % winners_n;

    std::vector<chips_t> amounts;
    for (int i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        amounts.push_back(amount);
    }
    return amounts;
}
} // namespace poker::algo