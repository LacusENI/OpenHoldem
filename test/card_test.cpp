#include "Card.hpp"

#include <gtest/gtest.h>

using namespace poker;

// 测试 toString(Card)
TEST(CardTest, ToStringFunction) {
    Card card1(Rank::ACE, Suit::CLUB);

    EXPECT_EQ("Ac", to_string(card1));

    Card card2(Rank::KING, Suit::DIAMOND);

    EXPECT_EQ("Kd", to_string(card2));
}

TEST(CardTest, FromStringBuilder) {
    EXPECT_EQ(Card("Ac"), Card(Rank::ACE, Suit::CLUB));
    EXPECT_EQ(Card("Kd"), Card(Rank::KING, Suit::DIAMOND));
    EXPECT_EQ(Card("Th"), Card(Rank::TEN, Suit::HEART));
    EXPECT_EQ(Card("8s"), Card(Rank::EIGHT, Suit::SPADE));
}

// 越界的点数/花色不应越界访问，返回占位符（阶段 0 防御）
TEST(CardTest, ToStringHandlesOutOfRange) {
    Card invalid(static_cast<Rank>(99), static_cast<Suit>(9));
    EXPECT_EQ("?_", to_string(invalid));
}