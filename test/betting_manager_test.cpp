#include <gtest/gtest.h>

#include <vector>

#include "BettingManager.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

namespace {
struct BlindRig {
    PlayerSet player_set;
    SeatManager seat_manager{&player_set};
    BettingManager betting_manager{&player_set};

    void setup(const std::vector<chips_t>& stacks, seat_t btn) {
        player_set.resize(static_cast<int>(stacks.size()));
        for (int s = 0; s < player_set.playerCount(); ++s) {
            player_set.playerAt(s).setChips(stacks[s]);
        }
        seat_manager.setBtnSeat(btn);
        betting_manager.setupBetting();
    }
};
} // namespace

// 正常筹码：小盲 5、大盲 10、bring-in 与最小加注基准为 10
TEST(BettingManagerTest, NormalBlinds) {
    BlindRig rig;
    rig.setup({1000, 1000, 1000}, 0); // btn=0, SB=1, BB=2

    EXPECT_EQ(5, rig.betting_manager.applySmallBlind(1));
    EXPECT_EQ(10, rig.betting_manager.applyBigBlind(2));

    EXPECT_EQ(5, rig.betting_manager.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(10, rig.betting_manager.getMinRaiseDelta());
}

// 短筹码小盲：只投入全部筹码，不影响大盲 bring-in
TEST(BettingManagerTest, ShortSmallBlindPostsAllChips) {
    BlindRig rig;
    rig.setup({1000, 3, 1000}, 0);

    EXPECT_EQ(3, rig.betting_manager.applySmallBlind(1));
    EXPECT_EQ(10, rig.betting_manager.applyBigBlind(2));

    EXPECT_EQ(3, rig.betting_manager.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(10, rig.betting_manager.getMinRaiseDelta());
}

// 短筹码大盲：投入全部筹码，但名义大盲仍作为 bring-in 与最小加注基准
TEST(BettingManagerTest, ShortBigBlindKeepsNominalBringIn) {
    BlindRig rig;
    rig.setup({1000, 1000, 7}, 0);

    EXPECT_EQ(5, rig.betting_manager.applySmallBlind(1));
    EXPECT_EQ(7, rig.betting_manager.applyBigBlind(2));

    EXPECT_EQ(7, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(10, rig.betting_manager.getMinRaiseDelta());
}

// 唯一最高注者多出的部分即未跟注额（弃牌者不计为跟注）
TEST(BettingManagerTest, FindUncalledBetUniqueTop) {
    BlindRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    rig.player_set.playerAt(0).setBet(200);
    rig.player_set.playerAt(1).setBet(100);
    rig.player_set.playerAt(1).allIn();
    rig.player_set.playerAt(2).fold();

    auto uncalled = rig.betting_manager.findUncalledBet();
    ASSERT_TRUE(uncalled.has_value());
    EXPECT_EQ(0, uncalled->seat);
    EXPECT_EQ(100, uncalled->amount);
}

// 两人并列最高注：没有未跟注部分
TEST(BettingManagerTest, FindUncalledBetNoneWhenTopTied) {
    BlindRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    rig.player_set.playerAt(0).setBet(100);
    rig.player_set.playerAt(1).setBet(100);
    rig.player_set.playerAt(2).setBet(50);

    EXPECT_FALSE(rig.betting_manager.findUncalledBet().has_value());
}

// 全员过牌（全为 0）：没有未跟注部分
TEST(BettingManagerTest, FindUncalledBetNoneOnAllChecks) {
    BlindRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    EXPECT_FALSE(rig.betting_manager.findUncalledBet().has_value());
}
