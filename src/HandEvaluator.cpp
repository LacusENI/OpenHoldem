#include <vector>
#include <algorithm>
#include "HandEvaluator.h"

namespace holdem::internal {

std::vector<Cards5> getCombinations(const Cards7& hand7) {
    std::vector<Cards5> combinations;

    std::vector<bool> mask = {false, false, true, true, true, true, true};

    do {
        Cards5 combo;
        int combo_i = 0;
        for (int i = 0; i < mask.size(); i++) {
            if (mask[i]) { combo[combo_i++] = hand7[i]; }
        }
        combinations.push_back(combo);
    } while (std::next_permutation(mask.begin(), mask.end()));

    return combinations;
}

SortedCounts getSortedCounts(Cards5 hand5) {
    std::sort(hand5.begin(), hand5.end(), std::greater());
    SortedCounts counts;
    counts.emplace_back(hand5[0].rank, 0);
    for (Card card : hand5) {
        if (card.rank != counts.back().rank) {
            counts.emplace_back(card.rank, 1);
        } else {
            counts.back().count++;
        }
    }
    std::sort(counts.begin(), counts.end(), std::greater());
    return counts;
}

/**
 * 判断5张牌是否组成同花
 * @param hand5 5张牌
 * @return true 若5张牌是同花
 */
bool isFlush(const Cards5& hand5) {
    Suit suit = hand5[0].suit;
    return std::all_of(hand5.begin(), hand5.end(),
        [suit](Card c) { return c.suit == suit; });
}

/**
 * 判断5张牌是否组成顺子（需要点数统计），
 * 如果是A2345顺子，调整牌的顺序（A视为点数最低牌）
 * @param counts 5张牌点数统计
 * @return true 若5张牌是顺子
 */
bool isStraight(SortedCounts& counts) {
    if (counts.size() != 5) return false;

    if (static_cast<int>(counts[0].rank) -
        static_cast<int>(counts[4].rank) == 4) {
        return true;
        }

    // 处理A2345顺子, 此时A作为点数最小牌
    if (counts[0].rank == Rank::ACE &&
        counts[1].rank == Rank::FIVE &&
        counts[4].rank == Rank::TWO) {
        // 将A移动到统计列表的最后一位(点数最小)
        std::rotate(counts.begin(), counts.begin() + 1, counts.end());
        return true;
        }
    return false;
}

/**
 * 判断有重复点数的牌型(对子/两对/三条/四条/葫芦)
 * @param counts 牌的点数统计
 * @return 牌型
 */
HandType evalPatternHand(const SortedCounts& counts) {
    std::vector table = {
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

/**
 * 判断无重复点数的牌型(高牌/顺子/同花/同花顺)
 * @param hand5 5张牌
 * @param counts 牌的点数统计
 * @return 牌型
 */
HandType evalUniqueHand(const Cards5& hand5, SortedCounts& counts) {
    bool flushFlag = isFlush(hand5);
    bool straightFlag = isStraight(counts);
    if (flushFlag && straightFlag) {
        // 最高为A的同花顺，判定为皇家同花顺
        if (counts[0].rank == Rank::ACE)
            return HandType::ROYAL_FLUSH;
        return HandType::STRAIGHT_FLUSH;
    }
    if (flushFlag) return HandType::FLUSH;
    if (straightFlag) return HandType::STRAIGHT;
    return HandType::HIGH_CARD;
}

/* 判断5张牌的牌型 */
HandType getHandType(const Cards5& hand5, SortedCounts& counts) {
    if (counts.size() != 5)
        return evalPatternHand(counts);
    return evalUniqueHand(hand5, counts);
}
}

namespace holdem {
HandValue HandEvaluator::getHandValue(const Cards5 &hand) {
    internal::SortedCounts counts = internal::getSortedCounts(hand);
    HandType handType = getHandType(hand, counts);
    int handValue = static_cast<int>(handType);
    int i = 0;
    for (; i < counts.size(); i++) {
        auto [rank, _] = counts[i];
        handValue = (handValue << 4) + static_cast<int>(rank);
    }
    for (; i < 5; i++) {
        handValue = handValue << 4;
    }
    return HandValue(handValue);
}

Cards5 HandEvaluator::selectBest(const Cards7 &hand7) {
    auto maxHandValue = HandValue(0);
    Cards5 bestHand;
    for (Cards5 hand5 : internal::getCombinations(hand7)) {
        HandValue handValue = getHandValue(hand5);
        if (handValue > maxHandValue) {
            maxHandValue = handValue;
            bestHand = hand5;
        }
    }
    return bestHand;
}

HandValue HandEvaluator::evaluate(const Cards7& hand7) {
    return getHandValue(selectBest(hand7));
}
}
