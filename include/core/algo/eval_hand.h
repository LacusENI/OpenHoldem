#pragma once

#include "Card.h"
#include "HandValue.h"

namespace poker::algo {
/**
 * 计算该玩家的手牌大小
 * @param seven_cards 5张公共牌+2张底牌
 * @return 手牌分值
*/
HandValue eval_hand(cards_7_t seven_cards);
} // namespace poker::math

namespace poker::internal::eval_hand {
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
 * @param five_cards 5张牌
 * @return 频次统计(按频次、点数从高到低依次排列)
 */
HandCounts eval_hand_counts(cards_5_t five_cards);

/**
 * 判断5张出牌的牌型
 * @param five_cards 5张出牌
 * @param counts 其点数统计
 * @return 5张出牌的牌型
 */
HandType eval_hand_type(const cards_5_t& five_cards, const HandCounts& counts);

/**
 * @brief 计算玩家出牌对应的编码
 * @param hand 玩家的出牌
 * @return 出牌对应的编码
 */
HandValue eval_hand_value(const cards_5_t& hand);

/**
 * 枚举所有可能的出牌组合(7选5)
 * @param seven_cards 7张手牌
 * @return 所有可能的5张出牌组合
 */
std::vector<cards_5_t> enum_5_from_7(const cards_7_t& seven_cards);
} // namespace poker::internal::eval_hand