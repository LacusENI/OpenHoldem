#include <gtest/gtest.h>
#include "HandEvaluator.h"

using namespace holdem;
using namespace holdem::internal;

// 测试生成的组合的数量是否正确
TEST(HandEvaluatorTest, TestCombinationSize) {
    Hand7 hand7 = {
        Card(Suit::CLUBS, Rank::ACE),
        Card(Suit::HEARTS, Rank::TWO),
        Card(Suit::DIAMONDS, Rank::EIGHT),
        Card(Suit::CLUBS, Rank::THREE),
        Card(Suit::HEARTS, Rank::JACK),
    };
    auto combos = getCombinations(hand7);

    EXPECT_EQ(combos.size(), 21);
}

// 测试生成的组合是否不重复
TEST(HandEvaluatorTest, TestCombinationsUniqueness) {
    Hand7 hand7 = {
        Card(Suit::CLUBS, Rank::ACE),
        Card(Suit::HEARTS, Rank::TWO),
        Card(Suit::DIAMONDS, Rank::EIGHT),
        Card(Suit::CLUBS, Rank::THREE),
        Card(Suit::HEARTS, Rank::JACK),
        Card(Suit::DIAMONDS, Rank::KING),
        Card(Suit::HEARTS, Rank::QUEEN)
    };

    auto combos = getCombinations(hand7);
    std::set<Hand5> uniqueCombos;
    for (auto combo : combos) {
        std::sort(combo.begin(), combo.end());
        uniqueCombos.insert(combo);
    }

    EXPECT_EQ(uniqueCombos.size(), 21);
}

TEST(HandEvaluatorTest, TestGetCounts) {
    Hand5 highCard = {
        Card(Suit::CLUBS, Rank::ACE),
        Card(Suit::HEARTS, Rank::TWO),
        Card(Suit::DIAMONDS, Rank::EIGHT),
        Card(Suit::CLUBS, Rank::THREE),
        Card(Suit::HEARTS, Rank::JACK),
    }; // A, 2, 8, 3, J -> A, J, 8, 3, 2
    auto resultHighCard = getCounts(highCard);
    ASSERT_EQ(resultHighCard.size(), 5);
    EXPECT_EQ(resultHighCard[0], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultHighCard[1], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultHighCard[2], RankCount(Rank::EIGHT, 1));
    EXPECT_EQ(resultHighCard[3], RankCount(Rank::THREE, 1));
    EXPECT_EQ(resultHighCard[4], RankCount(Rank::TWO, 1));

    Hand5 twoPair = {
        Card(Suit::CLUBS, Rank::ACE),
        Card(Suit::CLUBS, Rank::TWO),
        Card(Suit::DIAMONDS, Rank::THREE),
        Card(Suit::CLUBS, Rank::THREE),
        Card(Suit::HEARTS, Rank::JACK),
    }; // A, 2, 3, 3, J -> 3, 3, A, J, 2
    auto resultTwoPair = getCounts(twoPair);
    ASSERT_EQ(resultTwoPair.size(), 4);
    EXPECT_EQ(resultTwoPair[0], RankCount(Rank::THREE, 2));
    EXPECT_EQ(resultTwoPair[1], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultTwoPair[2], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultTwoPair[3], RankCount(Rank::TWO, 1));

    Hand5 fullHouse = {
        Card(Suit::CLUBS, Rank::EIGHT),
        Card(Suit::HEARTS, Rank::FOUR),
        Card(Suit::DIAMONDS, Rank::EIGHT),
        Card(Suit::CLUBS, Rank::FOUR),
        Card(Suit::HEARTS, Rank::FOUR),
    }; // 8, 4, 8, 4, 4 -> 4, 4, 4, 8, 8
    auto resultFullHouse = getCounts(fullHouse);
    ASSERT_EQ(resultFullHouse.size(), 2);
    EXPECT_EQ(resultFullHouse[0], RankCount(Rank::FOUR, 3));
    EXPECT_EQ(resultFullHouse[1], RankCount(Rank::EIGHT, 2));
}