#pragma once

#include <vector>

#include "GameTypes.h"

namespace poker::algo {

struct BettingContext {
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
};

struct BettingInput {
    chips_t target_bet;
    bool is_fold;
};

struct BettingResult {
    chips_t bet_delta;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
    ActionType action_type;
    bool is_target_bet_valid;
    bool has_sufficient_chips;
};

BettingResult process_betting(BettingContext context, BettingInput input);

std::vector<chips_t> calculate_awards(int pot, int winners_n);
} // namespace poker::math