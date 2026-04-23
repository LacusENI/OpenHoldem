#include <gtest/gtest.h>

#include "models/Card.h"

using namespace holdem;

// 测试 Card 的 toString() 方法
TEST(CardTest, ToStringFunction) {
    Card card1(Suit::CLUB, Rank::ACE);

    EXPECT_EQ("CA", card1.toString());

    Card card2(Suit::DIAMOND, Rank::KING);

    EXPECT_EQ("DK", card2.toString());
}

TEST(CardTest, FromStringBuilder) {
    EXPECT_EQ(Card("CA"), Card(Suit::CLUB, Rank::ACE));
    EXPECT_EQ(Card("DK"), Card(Suit::DIAMOND, Rank::KING));
    EXPECT_EQ(Card("HT"), Card(Suit::HEART, Rank::TEN));
    EXPECT_EQ(Card("S8"), Card(Suit::SPADE, Rank::EIGHT));
}