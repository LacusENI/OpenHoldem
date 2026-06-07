#include <algorithm>

#include "eval_hand.h"

namespace poker::internal::eval_hand {
std::vector<cards_5_t> enum_5_from_7(const cards_7_t& hand7) {
    std::vector<cards_5_t> combos;
    std::vector mask = {0, 0, 1, 1, 1, 1, 1};
    do {
        cards_5_t combo;
        int combo_i = 0;
        for (int i = 0; i < mask.size(); i++) {
            if (mask[i]) { combo[combo_i++] = hand7[i]; }
        }
        combos.push_back(combo);
    } while (std::ranges::next_permutation(mask).found);
    return combos;
}

HandValue eval_hand_value(const cards_5_t& hand) {
    HandCounts counts = eval_hand_counts(hand);
    HandType hand_type = eval_hand_type(hand, counts);
    std::vector<Rank> ranks;
    for (auto [rank, _] : counts) {
        ranks.push_back(rank);
    }
    return HandValue::build(hand_type, ranks);
}

HandCounts eval_hand_counts(cards_5_t hand5) {
    std::ranges::sort(hand5, std::greater());
    HandCounts counts;
    counts.emplace_back(hand5[0].rank, 0);
    for (Card card : hand5) {
        if (card.rank != counts.back().rank) {
            counts.emplace_back(card.rank, 1);
        } else {
            counts.back().count++;
        }
    }
    std::ranges::sort(counts, std::greater());

    // 处理A2345顺子
    if (counts.size() == 5 &&
        counts[0].rank == Rank::ACE &&
        counts[1].rank == Rank::FIVE &&
        counts[4].rank == Rank::TWO) {
        std::ranges::rotate(counts, counts.begin() + 1);
    }
    return counts;
}

/**
 * 判断5张牌是否组成同花
 * @param hand5 5张牌
 * @return true 若5张牌是同花
 */
bool isFlush(const cards_5_t& hand5) {
    Suit suit = hand5[0].suit;
    return std::ranges::all_of(hand5,
        [suit](Card c) { return c.suit == suit; });
}

/**
 * 判断5张牌是否组成顺子（需要点数统计），
 * @param counts 5张牌点数统计
 * @return true 若5张牌是顺子
 */
bool isStraight(const HandCounts& counts) {
    if (counts.size() != 5) return false;

    if (static_cast<int>(counts[0].rank) -
        static_cast<int>(counts[4].rank) == 4) {
        return true;
        }

    // 处理A2345顺子
    if (counts[4].rank == Rank::ACE &&
        counts[0].rank == Rank::FIVE &&
        counts[3].rank == Rank::TWO) {
        return true;
        }
    return false;
}

/**
 * 判断无重复点数的牌型(高牌/顺子/同花/同花顺)
 * @param hand5 5张牌
 * @param counts 牌的点数统计
 * @return 牌型
 */
HandType evalUniqueHandType(const cards_5_t& hand5, const HandCounts& counts) {
    bool is_flush = isFlush(hand5);
    bool is_straight = isStraight(counts);
    if (is_flush && is_straight) {
        // 最高为A的同花顺，判定为皇家同花顺
        if (counts[0].rank == Rank::ACE)
            return HandType::ROYAL_FLUSH;
        return HandType::STRAIGHT_FLUSH;
    }
    if (is_flush) return HandType::FLUSH;
    if (is_straight) return HandType::STRAIGHT;
    return HandType::HIGH_CARD;
}

/**
 * 判断有重复点数的牌型(对子/两对/三条/四条/葫芦)
 * @param counts 牌的点数统计
 * @return 牌型
 */
HandType evalPatternHandType(const HandCounts& counts) {
    const std::vector table = {
        HandType::ONE_PAIR,
        HandType::TWO_PAIR,
        HandType::THREE_OF_A_KIND,
        HandType::FULL_HOUSE,
        HandType::FOUR_OF_A_KIND,
    };
    int max1 = counts[0].count;
    int max2 = counts[1].count;
    return table[2*(max1 - 2) + (max2 - 1)];
}

HandType eval_hand_type(const cards_5_t& hand5, const HandCounts& counts) {
    if (counts.size() != 5)
        return evalPatternHandType(counts);
    return evalUniqueHandType(hand5, counts);
}
} // namespace poker::internal::eval_hand

namespace poker::math {
HandValue eval_hand(cards_7_t seven_cards) {
    using namespace internal::eval_hand;

    // 枚举21个五牌组合, 选择牌值最大者
    auto best_hand_value = HandValue(0);
    for (cards_5_t five_cards : enum_5_from_7(seven_cards)) {
        HandValue handValue = eval_hand_value(five_cards);
        best_hand_value = std::max(best_hand_value, handValue);
    }

    return best_hand_value;
}
} // namespace poker::math