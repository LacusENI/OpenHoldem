#ifndef OPENHOLDEM_HAND_EVALUATOR_H
#define OPENHOLDEM_HAND_EVALUATOR_H
#include <array>
#include "Card.h"

namespace holdem {

using Hand5 = Cards5;
using Hand7 = Cards7;

enum class HandType {
    HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT,
    FLUSH, FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH
};

/**
 * @brief 表示手牌强度的分值类型
 *
 * 采用 32 位整数编码, 整数越大，牌越大, 格式为：0xTPSKKK,
 * - T: HandType (4 bits)       牌型等级(0-9)
 * - P: Primary Rank (4 bits)   主牌点数(如葫芦的三条的点数)
 * - S: Secondary Rank (4 bits) 次高牌点数(如葫芦的对子的点数）
 * - K：Kickers (4 bits each)   踢脚牌点数(按从大到小排序)
 * - 按照比较顺序排列，不足的位置设置为零
 * - 如: 葫芦: PS000，两条: PKKK0，两对：PSK00
 */
struct HandValue {
    uint32_t raw_value;

    HandValue() : raw_value(0) {}
    explicit HandValue(int raw_value) : raw_value(raw_value) {}

    [[nodiscard]] HandType getHandType() const {
        return static_cast<HandType>(raw_value >> 20);
    }

    [[nodiscard]] Rank getRank(const int index) const {
        return static_cast<Rank>((raw_value >> (16 - 4 * index)) & 0xF);
    }

    bool operator==(const HandValue &other) const {
        return raw_value == other.raw_value;
    }

    bool operator>(const HandValue &other) const {
        return raw_value > other.raw_value;
    }

    bool operator<(const HandValue &other) const {
        return raw_value < other.raw_value;
    }
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
    static HandValue getHandValue(const Hand5& hand);

    /**
     * 从玩家7张手牌中选取最好的5张手牌
     * @param hand7 玩家的手牌 (公共牌+底牌)
     * @return 玩家的最佳出牌
     */
    static Hand5 selectBest(const Hand7& hand7);

    static HandValue evaluate(const Hand7& hand7);
};
} // namespace holdem

namespace holdem::internal {

/**
 * 枚举所有可能的出牌组合(7选5)
 * @param hand7 7张手牌
 * @return 所有可能的5张出牌组合
 */
std::vector<Hand5> getCombinations(const Hand7& hand7);

/**
 * @brief 记录频次统计中的某一点数及其频次的数据结构
 */
struct RankCount {
    Rank rank;
    int count;

    RankCount() : rank(), count() {};
    RankCount(Rank rank, int count) : rank(rank), count(count) {}

    bool operator>(const RankCount& other) const {
        if (count != other.count) return count > other.count;
        return rank > other.rank;
    }

    bool operator==(const RankCount& other) const {
        return count == other.count && rank == other.rank;
    }
};

using SortedCounts = std::vector<RankCount>;

/**
 * 统计5张牌的频次，按频次从高到低排序，若频次相同，点数高者优先，如：
 * - 5, 5, 4, 4, 2 -> 5:2, 4:2, 2:1
 * - 6, 6, 6, 9, 9 -> 6:3, 9:2
 * @param hand5 5张牌
 * @return 频次统计(按频次、点数从高到低依次排列)
 */
SortedCounts getSortedCounts(Hand5 hand5);
} // namespace holdem::internal
#endif //OPENHOLDEM_HAND_EVALUATOR_H