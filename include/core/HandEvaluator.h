#pragma once

#include <vector>

#include "Card.h"
#include "HandValue.h"

namespace holdem {

namespace utils {
/* 将2张牌和5张牌数组拼接成一个7张牌数组 */
Cards7 concatCards(const Cards2& cards2, const Cards5& cards5);
}

/**
 * @brief 扑克牌型评估器
 */
class HandEvaluator {
public:
    /**
     * 计算该玩家的手牌大小
     * @param hand7 5张公共牌+2张底牌
     * @return 手牌分值
     */
    static HandValue evaluate(const Cards7& hand7);
};
} // namespace holdem

namespace holdem::internal {

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

using HandCounts = std::vector<RankCount>;

/**
 * 统计5张牌的频次，按频次从高到低排序，若频次相同，点数高者优先，如：
 * - 5, 5, 4, 4, 2 -> 5:2, 4:2, 2:1
 * - 6, 6, 6, 9, 9 -> 6:3, 9:2
 * @note 如果是A2345顺子，则A视为点数最低牌
 * @param hand5 5张牌
 * @return 频次统计(按频次、点数从高到低依次排列)
 */
HandCounts evalHandCounts(Cards5 hand5);

/**
 * @brief 计算玩家出牌对应的编码
 * @param hand 玩家的出牌
 * @return 出牌对应的编码
 */
HandValue evalHandValue(const Cards5& hand);

/**
 * 判断5张出牌的牌型
 * @param hand5 5张出牌
 * @param counts 其点数统计
 * @return 5张出牌的牌型
 */
HandType evalHandType(const Cards5& hand5, const HandCounts& counts);

/**
 * 枚举所有可能的出牌组合(7选5)
 * @param hand7 7张手牌
 * @return 所有可能的5张出牌组合
 */
std::vector<Cards5> enum5from7(const Cards7& hand7);
} // namespace holdem::internal
