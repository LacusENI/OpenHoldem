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
    bool is_allin = false;  // 玩家是否已 all-in

    Player() = default;
    explicit Player(Position position) : position(position) {}

    /* 玩家是否在场(未弃牌) */
    bool isActive() const { return !is_folded; }
    /* 玩家是否能继续行动(未弃牌也未 all-in) */
    bool canAct() const { return !is_folded && !is_allin; }
};

}
