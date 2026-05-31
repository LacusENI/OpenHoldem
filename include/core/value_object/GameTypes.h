#pragma once

namespace poker {
using seat_idx_t = int; // 玩家座位
using chips_t = int;    // 筹码数量

enum class GameState {
    IDLE, PREFLOP, FLOP, TURN, RIVER, AWARD
};

enum class ActionType {
    VOID, INVALID,
    CALL, CHECK,
    RAISE, BET,
    FOLD, ALL_IN,
    SMALL_BLIND,
    BIG_BLIND
};

struct Action {
    seat_idx_t actor_position;
    ActionType type;
    chips_t amount;
};
}
