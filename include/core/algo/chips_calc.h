#pragma once

#include <vector>

#include "GameTypes.hpp"

namespace poker::algo {

struct BettingContext {
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
    bool can_raise = true;
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
    Action action;
    bool is_target_bet_valid;
    bool has_sufficient_chips;
};

BettingResult process_betting(BettingContext context, BettingInput input);

std::vector<chips_t> calculate_awards(int pot, int winners_n);
} // namespace poker::algo