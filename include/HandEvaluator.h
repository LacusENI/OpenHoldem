#ifndef OPENHOLDEM_HANDEVALUATOR_H
#define OPENHOLDEM_HANDEVALUATOR_H
#include <array>
#include "Card.h"

namespace holdem {

using Hand5 = std::array<Card, 5>;
using Hand7 = std::array<Card, 7>;

/**
 * @brief 表示手牌强度的分值类型
 *
 * 采用 32 位整数编码, 整数越大，牌越大, 格式为：0xTPSKKK,
 * - T: HandType (4 bits) 牌型等级(0-9)
 * - P: Primary Rank (4 bits) 主牌点数(如葫芦的三条的点数)
 * - S: Secondary Rank (4 bits) 次高牌点数(如葫芦的对子的点数）
 * - K：Kickers (4 bits each) 踢脚牌点数(按从大到小排序)
 */
using HandValue = uint32_t;

enum class HandType {
    HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT,
    FLUSH, FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH
};

/**
 * @brief 扑克牌型评估器
 */
class HandEvaluator {
public:
    /**
     * 计算玩家出牌对应的编码
     * @param hand 玩家的出牌
     * @return 出牌对应的编码
     */
    static HandValue getHandValue(const Hand5 &hand);

    /**
     * 从玩家7张手牌中选取最好的5张手牌
     * @param hand7 玩家的手牌 (公共牌+私有牌)
     * @return 玩家的最佳出牌
     */
    static Hand5 selectBest(Hand7& hand7);
};
} // namespace holdem

#endif //OPENHOLDEM_HANDEVALUATOR_H