#ifndef OPENHOLDEM_GAME_TYPES_H
#define OPENHOLDEM_GAME_TYPES_H

namespace holdem {
using PlayerId = int; // 玩家ID
using Position = int; // 玩家座位
using Stack = int;    // 筹码数量

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

#endif //OPENHOLDEM_GAME_TYPES_H