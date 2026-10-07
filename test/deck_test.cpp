#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>

#include "Card.hpp"
#include "Deck.hpp"

using namespace poker;

// 指定种子可被读出
TEST(DeckTest, ReportsSeed) {
    Deck deck(123456);
    EXPECT_EQ(123456u, deck.getSeed());
}

// 相同种子 → 相同的洗牌/发牌序列（可复现）
TEST(DeckTest, SameSeedReproducesSameDeal) {
    Deck a(42);
    Deck b(42);
    a.shuffle();
    b.shuffle();
    for (int i = 0; i < 52; ++i) {
        EXPECT_EQ(a.deal(), b.deal()) << "i=" << i;
    }
}

// 不同种子 → 洗牌序列不同（52 张全同的概率可忽略）
TEST(DeckTest, DifferentSeedsDiffer) {
    Deck a(1);
    Deck b(2);
    a.shuffle();
    b.shuffle();
    bool any_diff = false;
    for (int i = 0; i < 52; ++i) {
        if (!(a.deal() == b.deal())) any_diff = true;
    }
    EXPECT_TRUE(any_diff);
}

// 发完 52 张后再发应抛异常
TEST(DeckTest, DealBeyondSizeThrows) {
    Deck deck(1);
    for (int i = 0; i < 52; ++i) {
        deck.deal();
    }
    EXPECT_THROW(deck.deal(), std::runtime_error);
}
