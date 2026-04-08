#include <gtest/gtest.h>
#include "HandEvaluator.h"

using namespace holdem;
using namespace holdem::internal;

// 测试生成的组合的数量是否正确
TEST(HandEvaluatorTest, CombinationSize) {
    Cards7 hand7 = {
        Card("CA"), Card("H2"), Card("D8"), Card("C3"),
        Card("HJ"), Card("DK"), Card("HQ")
    };
    auto combos = getCombinations(hand7);

    EXPECT_EQ(combos.size(), 21);
}

// 测试生成的组合是否不重复
TEST(HandEvaluatorTest, CombinationsUniqueness) {
    Cards7 hand7 = {
        Card("CA"), Card("H2"), Card("D8"), Card("C3"),
        Card("HJ"), Card("DK"), Card("HQ")
    };

    auto combos = getCombinations(hand7);
    std::set<Cards5> uniqueCombos;
    for (auto combo : combos) {
        std::sort(combo.begin(), combo.end());
        uniqueCombos.insert(combo);
    }

    EXPECT_EQ(uniqueCombos.size(), 21);
}

// 测试点数频次统计是否正确
TEST(HandEvaluatorTest, GetSortedCounts) {
    Cards5 highCard = {
        Card("CA"), Card("H2"), Card("D8"), Card("C3"), Card("HJ")
    }; // A, 2, 8, 3, J -> A, J, 8, 3, 2
    auto resultHighCard = getSortedCounts(highCard);
    ASSERT_EQ(resultHighCard.size(), 5);
    EXPECT_EQ(resultHighCard[0], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultHighCard[1], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultHighCard[2], RankCount(Rank::EIGHT, 1));
    EXPECT_EQ(resultHighCard[3], RankCount(Rank::THREE, 1));
    EXPECT_EQ(resultHighCard[4], RankCount(Rank::TWO, 1));

    Cards5 twoPair = {
        Card("CA"), Card("H2"), Card("D3"), Card("C3"), Card("HJ")
    }; // A, 2, 3, 3, J -> 3, 3, A, J, 2
    auto resultTwoPair = getSortedCounts(twoPair);
    ASSERT_EQ(resultTwoPair.size(), 4);
    EXPECT_EQ(resultTwoPair[0], RankCount(Rank::THREE, 2));
    EXPECT_EQ(resultTwoPair[1], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultTwoPair[2], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultTwoPair[3], RankCount(Rank::TWO, 1));

    Cards5 fullHouse = {
        Card("C8"), Card("H4"), Card("D8"), Card("C4"), Card("D4"),
    }; // 8, 4, 8, 4, 4 -> 4, 4, 4, 8, 8
    auto resultFullHouse = getSortedCounts(fullHouse);
    ASSERT_EQ(resultFullHouse.size(), 2);
    EXPECT_EQ(resultFullHouse[0], RankCount(Rank::FOUR, 3));
    EXPECT_EQ(resultFullHouse[1], RankCount(Rank::EIGHT, 2));
}

// 测试牌型判断是否正确
TEST(HandEvaluatorTest, EvaluateHandType) {
    Cards5 flush = {
        Card("HA"), Card("H2"), Card("HJ"), Card("H7"), Card("HK")
    }; // 同花 A, 2, J, 7, K
    HandValue flushValue = HandEvaluator::getHandValue(flush);
    EXPECT_EQ(flushValue.getHandType(), HandType::FLUSH);

    Cards5 fullHouse = {
        Card("C8"), Card("H8"),Card("S5"),Card("D5"),Card("C5"),
    }; // 葫芦 8, 8, 5, 5, 5
    HandValue fullHouseValue = HandEvaluator::getHandValue(fullHouse);
    EXPECT_EQ(fullHouseValue.getHandType(), HandType::FULL_HOUSE);

    Cards5 straight = {
        Card("C2"), Card("HA"), Card("H4"), Card("D3"), Card("S5"),
    }; // 顺子 2, A, 4, 3, 5
    HandValue straightValue = HandEvaluator::getHandValue(straight);
    EXPECT_EQ(straightValue.getHandType(), HandType::STRAIGHT);

    Cards5 royalFlush = {
        Card("ST"), Card("SA"), Card("SJ"), Card("SQ"), Card("SK")
    }; // 皇家同花顺
    HandValue royalFlushValue = HandEvaluator::getHandValue(royalFlush);
    EXPECT_EQ(royalFlushValue.getHandType(), HandType::ROYAL_FLUSH);
}

// 测试手牌分值计算是否正确
TEST(HandEvaluatorTest, GetHandValue) {
    Cards5 fullHouse = {
        Card("C8"), Card("H8"), Card("S5"), Card("D5"), Card("C5")
    }; // 葫芦 8, 8, 5, 5, 5 -> 5, 5, 5, 8, 8
    HandValue fullHouseValue = HandEvaluator::getHandValue(fullHouse);
    EXPECT_EQ(fullHouseValue.getHandType(), HandType::FULL_HOUSE);
    EXPECT_EQ(fullHouseValue.getRank(0), Rank::FIVE);
    EXPECT_EQ(fullHouseValue.getRank(1), Rank::EIGHT);

    Cards5 highCard = {
        Card("C8"), Card("H9"), Card("ST"), Card("DQ"), Card("C3")
    }; // 高牌 8, 9, 10, Q, 3 -> Q, 10, 9, 8, 3
    HandValue highCardValue = HandEvaluator::getHandValue(highCard);
    EXPECT_EQ(highCardValue.getHandType(), HandType::HIGH_CARD);
    EXPECT_EQ(highCardValue.getRank(0), Rank::QUEEN);
    EXPECT_EQ(highCardValue.getRank(1), Rank::TEN);
    EXPECT_EQ(highCardValue.getRank(2), Rank::NINE);
    EXPECT_EQ(highCardValue.getRank(3), Rank::EIGHT);
    EXPECT_EQ(highCardValue.getRank(4), Rank::THREE);

    Cards5 straight = {
        Card("C2"), Card("HA"), Card("H4"), Card("D3"), Card("S5")
    }; // 顺子 2, A, 4, 3, 5 -> 5, 4, 3, 2, A
    HandValue straightValue = HandEvaluator::getHandValue(straight);
    EXPECT_EQ(straightValue.getHandType(), HandType::STRAIGHT);
    EXPECT_EQ(straightValue.getRank(0), Rank::FIVE);
    EXPECT_EQ(straightValue.getRank(1), Rank::FOUR);
    EXPECT_EQ(straightValue.getRank(2), Rank::THREE);
    EXPECT_EQ(straightValue.getRank(3), Rank::TWO);
    EXPECT_EQ(straightValue.getRank(4), Rank::ACE);

    Cards5 twoPair = {
        Card("C2"), Card("D2"), Card("ST"), Card("SQ"), Card("CT"),
    }; // 两对 2, 2, 10, Q, 10 -> 10, 10, 2, 2, Q
    HandValue twoPairValue = HandEvaluator::getHandValue(twoPair);
    EXPECT_EQ(twoPairValue.getHandType(), HandType::TWO_PAIR);
    EXPECT_EQ(twoPairValue.getRank(0), Rank::TEN);
    EXPECT_EQ(twoPairValue.getRank(1), Rank::TWO);
    EXPECT_EQ(twoPairValue.getRank(2), Rank::QUEEN);
}

TEST(HandEvaluatorTest, SelectBest) {
    Cards7 case1 = {
        Card("CK"), Card("CQ"), Card("SQ"), Card("DQ"),
        Card("CK"), Card("CA"), Card("CJ")
    };
    std::sort(case1.begin(), case1.end());
    Cards5 fullHouse = HandEvaluator::selectBest(case1);
    std::sort(fullHouse.begin(), fullHouse.end());
    EXPECT_EQ(fullHouse[0].rank, Rank::QUEEN);
    EXPECT_EQ(fullHouse[1].rank, Rank::QUEEN);
    EXPECT_EQ(fullHouse[2].rank, Rank::QUEEN);
    EXPECT_EQ(fullHouse[3].rank, Rank::KING);
    EXPECT_EQ(fullHouse[4].rank, Rank::KING);
}