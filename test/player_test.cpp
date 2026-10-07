#include <gtest/gtest.h>

#include "GameTypes.hpp"
#include "Player.hpp"

using namespace poker;

// 支付筹码：扣减并累加 committed
TEST(PlayerTest, PayChipsDeductsAndAccumulatesCommitted) {
    Player player;
    player.setChips(100);
    player.payChips(30);
    EXPECT_EQ(70, player.getChips());
    EXPECT_EQ(30, player.getCommitted());
    EXPECT_TRUE(player.isActive());
}

// 支付至 0 即 all-in
TEST(PlayerTest, PayAllChipsBecomesAllIn) {
    Player player;
    player.setChips(50);
    player.payChips(50);
    EXPECT_EQ(0, player.getChips());
    EXPECT_TRUE(player.isAllIn());
}

// 获得筹码：all-in 恢复到有余额则解除 all-in
TEST(PlayerTest, WinChipsLiftsAllIn) {
    Player player;
    player.setChips(50);
    player.payChips(50);
    ASSERT_TRUE(player.isAllIn());
    player.winChips(20);
    EXPECT_EQ(20, player.getChips());
    EXPECT_TRUE(player.isActive());
}

// commit：设置下注并支付，扣至 0 则 all-in
TEST(PlayerTest, CommitSetsBetAndMayGoAllIn) {
    Player player;
    player.setChips(100);
    EXPECT_TRUE(player.commit(40));
    EXPECT_EQ(40, player.getBet());
    EXPECT_EQ(60, player.getChips());
    EXPECT_EQ(40, player.getCommitted());
    EXPECT_TRUE(player.isActive());

    EXPECT_TRUE(player.commit(100));
    EXPECT_EQ(100, player.getBet());
    EXPECT_EQ(0, player.getChips());
    EXPECT_TRUE(player.isAllIn());
}

// commit 超出筹码返回 false 且不改变状态
TEST(PlayerTest, CommitRejectsOverCommit) {
    Player player;
    player.setChips(10);
    EXPECT_FALSE(player.commit(20));
    EXPECT_EQ(10, player.getChips());
    EXPECT_EQ(0, player.getBet());
    EXPECT_TRUE(player.isActive());
}

// 退款：恢复筹码/下注/投入并解除 all-in
TEST(PlayerTest, RefundLiftsAllInAndRestoresBet) {
    Player player;
    player.setChips(100);
    player.commit(100); // all-in
    ASSERT_TRUE(player.isAllIn());

    player.refund(30);
    EXPECT_EQ(30, player.getChips());
    EXPECT_EQ(70, player.getBet());
    EXPECT_EQ(70, player.getCommitted());
    EXPECT_TRUE(player.isActive());
}
