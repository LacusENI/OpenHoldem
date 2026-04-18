#ifndef OPENHOLDEM_PLAYER_H
#define OPENHOLDEM_PLAYER_H
#include "GameTypes.h"
#include "Card.h"
#include "Hand.h"

namespace holdem {
/**
 * @brief 玩家实体类
 */
struct Player {
    PlayerId id;       // 玩家ID
    Position position; // 玩家所在位置
    Cards2 hole_cards; // 玩家的底牌

    Stack chips = 0;        // 玩家拥有的筹码
    Stack current_bet = 0;  // 玩家当前下注额
    bool is_folded = false; // 玩家是否已弃牌

    HandValue hand_value; // 手牌分值
};

}

#endif //OPENHOLDEM_PLAYER_H