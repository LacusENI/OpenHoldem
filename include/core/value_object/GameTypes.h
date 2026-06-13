#pragma once

#include <string>

namespace poker {
using seat_idx_t = int; // 玩家座位
using chips_t = int;    // 筹码数量

enum class Street {
    IDLE, PREFLOP, FLOP, TURN, RIVER, AWARD
};

Street next_street(Street street);
std::string to_string(Street street);

enum class ActionType {
    VOID,
    CALL,
    CHECK,
    RAISE,
    BET,
    FOLD,
    ALL_IN,
};

struct Action {
    seat_idx_t actor_position;
    ActionType type;
    chips_t amount;
};
} // namespace poker
