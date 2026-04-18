#ifndef OPENHOLDEM_GAME_TYPES_H
#define OPENHOLDEM_GAME_TYPES_H
#include <string>

namespace holdem {
using PlayerId = int; // 玩家ID
using Position = int; // 玩家座位
using Stack = int;    // 筹码数量

struct Action {
    Position actor_position;
    std::string description;
    Stack amount;
};
}

#endif //OPENHOLDEM_GAME_TYPES_H