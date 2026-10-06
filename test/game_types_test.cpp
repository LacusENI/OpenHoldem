#include <gtest/gtest.h>

#include "GameTypes.hpp"

using namespace poker;

// isPreflop 仅对 PREFLOP 为真（P2-1）
TEST(StageTest, IsPreflop) {
    EXPECT_TRUE(isPreflop(Stage::PREFLOP));
    EXPECT_FALSE(isPreflop(Stage::IDLE));
    EXPECT_FALSE(isPreflop(Stage::FLOP));
    EXPECT_FALSE(isPreflop(Stage::TURN));
    EXPECT_FALSE(isPreflop(Stage::RIVER));
    EXPECT_FALSE(isPreflop(Stage::SHOWDOWN));
    EXPECT_FALSE(isPreflop(Stage::SETTLEMENT));
}

// isPostflop 仅对 FLOP/TURN/RIVER 为真，不含 SHOWDOWN/SETTLEMENT（P2-1）
TEST(StageTest, IsPostflop) {
    EXPECT_TRUE(isPostflop(Stage::FLOP));
    EXPECT_TRUE(isPostflop(Stage::TURN));
    EXPECT_TRUE(isPostflop(Stage::RIVER));
    EXPECT_FALSE(isPostflop(Stage::IDLE));
    EXPECT_FALSE(isPostflop(Stage::PREFLOP));
    EXPECT_FALSE(isPostflop(Stage::SHOWDOWN));
    EXPECT_FALSE(isPostflop(Stage::SETTLEMENT));
}
