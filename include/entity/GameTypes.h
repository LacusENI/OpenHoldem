#pragma once

namespace holdem {
using PlayerId = int; // 玩家ID
using Position = int; // 玩家座位
using Stack = int;    // 筹码数量

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
    Position actor_position;
    ActionType type;
    Stack amount;
};
}
