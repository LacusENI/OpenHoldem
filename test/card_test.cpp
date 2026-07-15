#include "Card.hpp"

#include <gtest/gtest.h>

using namespace poker;

// 测试 Card 的 toString() 方法
TEST(CardTest, ToStringFunction) {
    Card card1(Suit::CLUB, Rank::ACE);

    EXPECT_EQ("CA", to_string(card1));

    Card card2(Suit::DIAMOND, Rank::KING);

    EXPECT_EQ("DK", to_string(card2));
}

TEST(CardTest, FromStringBuilder) {
    EXPECT_EQ(Card("CA"), Card(Suit::CLUB, Rank::ACE));
    EXPECT_EQ(Card("DK"), Card(Suit::DIAMOND, Rank::KING));
    EXPECT_EQ(Card("HT"), Card(Suit::HEART, Rank::TEN));
    EXPECT_EQ(Card("S8"), Card(Suit::SPADE, Rank::EIGHT));
}