#pragma once

#include "GameTypes.h"
#include "Card.h"

namespace holdem {
/**
 * @brief 玩家实体类
 */
struct Player {
    Position position; // 玩家所在位置
    Cards2 hole_cards; // 玩家的底牌

    Stack chips = 0;        // 玩家拥有的筹码
    bool is_folded = false; // 玩家是否已弃牌

    Player() = default;
    explicit Player(Position position) : position(position) {}
};

}
