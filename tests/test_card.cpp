#include <gtest/gtest.h>
#include "Card.h"

using namespace holdem;

TEST(CardTest, ToStringFunction) {
    Card card1(Suit::CLUBS, Rank::ACE);

    EXPECT_EQ("CA", card1.toString());

    Card card2(Suit::DIAMONDS, Rank::KING);

    EXPECT_EQ("DK", card2.toString());
}