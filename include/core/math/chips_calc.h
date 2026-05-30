#pragma once

#include "GameTypes.h"

namespace poker::math {

struct BettingContext {
    Stack player_chips;
    Stack player_bet;
    Stack round_bet;
    Stack min_raise_delta;
};

struct BettingInput {
    Stack target_bet;
    bool is_fold;
};

struct BettingResult {
    Stack bet_delta;
    Stack player_bet;
    Stack round_bet;
    Stack min_raise_delta;
    ActionType action_type;
    bool is_target_bet_valid;
    bool has_sufficient_chips;
};

BettingResult process_betting(BettingContext context, BettingInput input);

} // namespace poker::math