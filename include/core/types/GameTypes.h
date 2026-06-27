#pragma once

#include <string>

namespace poker {
using seat_idx_t = int; // 玩家座位
using chips_t = int;    // 筹码数量

enum class Stage {
    IDLE, PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, SETTLEMENT
};

Stage next_stage(Stage stage);
std::string to_string(Stage stage);

enum class Action {
    VOID,
    CALL,
    CHECK,
    RAISE,
    BET,
    FOLD,
    ALL_IN,
};

std::string to_string(Action action);
} // namespace poker
