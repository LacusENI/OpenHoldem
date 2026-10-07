#include <gtest/gtest.h>

#include <vector>

#include "BettingRound.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

namespace {
struct RoundRig {
    PlayerSet player_set;
    SeatManager seat_manager{player_set};
    BettingRound betting_round{player_set, seat_manager};

    void setup(const std::vector<chips_t>& stacks, seat_t btn) {
        player_set.resize(static_cast<int>(stacks.size()));
        for (int s = 0; s < player_set.playerCount(); ++s) {
            player_set.playerAt(s).setChips(stacks[s]);
        }
        seat_manager.setBtnSeat(btn);
        betting_round.resetBets();
    }

    void setup(int player_count, seat_t btn) {
        setup(std::vector<chips_t>(player_count, 1000), btn);
    }
};
} // namespace

// ===== 金额 / 盲注 =====

// 正常筹码：小盲 5、大盲 10、bring-in 与最小加注基准为 10
TEST(BettingRoundTest, NormalBlinds) {
    RoundRig rig;
    rig.setup({1000, 1000, 1000}, 0); // btn=0, SB=1, BB=2

    EXPECT_EQ(5, rig.betting_round.applySmallBlind(1));
    EXPECT_EQ(10, rig.betting_round.applyBigBlind(2));

    EXPECT_EQ(5, rig.betting_round.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_round.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_round.getRoundBet());
    EXPECT_EQ(10, rig.betting_round.getMinRaiseDelta());
}

// 短筹码小盲：只投入全部筹码，不影响大盲 bring-in
TEST(BettingRoundTest, ShortSmallBlindPostsAllChips) {
    RoundRig rig;
    rig.setup({1000, 3, 1000}, 0);

    EXPECT_EQ(3, rig.betting_round.applySmallBlind(1));
    EXPECT_EQ(10, rig.betting_round.applyBigBlind(2));

    EXPECT_EQ(3, rig.betting_round.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_round.getRoundBet());
    EXPECT_EQ(10, rig.betting_round.getMinRaiseDelta());
}

// 短筹码大盲：投入全部筹码，但名义大盲仍作为 bring-in 与最小加注基准
TEST(BettingRoundTest, ShortBigBlindKeepsNominalBringIn) {
    RoundRig rig;
    rig.setup({1000, 1000, 7}, 0);

    EXPECT_EQ(5, rig.betting_round.applySmallBlind(1));
    EXPECT_EQ(7, rig.betting_round.applyBigBlind(2));

    EXPECT_EQ(7, rig.betting_round.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_round.getRoundBet());
    EXPECT_EQ(10, rig.betting_round.getMinRaiseDelta());
}

// 唯一最高注者多出的部分即未跟注额（弃牌者不计为跟注）
TEST(BettingRoundTest, FindUncalledBetUniqueTop) {
    RoundRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    rig.player_set.playerAt(0).setBet(200);
    rig.player_set.playerAt(1).setBet(100);
    rig.player_set.playerAt(1).allIn();
    rig.player_set.playerAt(2).fold();

    auto uncalled = rig.betting_round.findUncalledBet();
    ASSERT_TRUE(uncalled.has_value());
    EXPECT_EQ(0, uncalled->seat);
    EXPECT_EQ(100, uncalled->amount);
}

// 两人并列最高注：没有未跟注部分
TEST(BettingRoundTest, FindUncalledBetNoneWhenTopTied) {
    RoundRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    rig.player_set.playerAt(0).setBet(100);
    rig.player_set.playerAt(1).setBet(100);
    rig.player_set.playerAt(2).setBet(50);

    EXPECT_FALSE(rig.betting_round.findUncalledBet().has_value());
}

// 全员过牌（全为 0）：没有未跟注部分
TEST(BettingRoundTest, FindUncalledBetNoneOnAllChecks) {
    RoundRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    EXPECT_FALSE(rig.betting_round.findUncalledBet().has_value());
}

// ===== 行动顺序 / 流 =====

// 翻前从 UTG 开始行动
TEST(BettingRoundTest, PreflopStartsAtUtg) {
    RoundRig rig;
    rig.setup(4, 0); // btn=0, SB=1, BB=2, UTG=3
    rig.betting_round.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(3, rig.betting_round.getActingPlayer());
}

// 翻后从按钮左手第一家开始行动
TEST(BettingRoundTest, PostflopStartsLeftOfButton) {
    RoundRig rig;
    rig.setup(4, 0); // 按钮下一家 = 1
    for (Stage stage : {Stage::FLOP, Stage::TURN, Stage::RIVER}) {
        rig.betting_round.setupToStart(stage);
        EXPECT_EQ(1, rig.betting_round.getActingPlayer());
    }
}

// 非下注阶段没有有效行动者，且不会沿用旧值
TEST(BettingRoundTest, NonBettingStageClearsActingPlayer) {
    RoundRig rig;
    rig.setup(3, 0);

    rig.betting_round.setupToStart(Stage::PREFLOP);
    ASSERT_NE(INVALID_SEAT, rig.betting_round.getActingPlayer());

    for (Stage stage : {Stage::IDLE, Stage::SHOWDOWN, Stage::SETTLEMENT}) {
        rig.betting_round.setupToStart(stage);
        EXPECT_EQ(INVALID_SEAT, rig.betting_round.getActingPlayer());
        EXPECT_FALSE(rig.betting_round.hasActingPlayer());
    }
}

// 翻后无人下注时，所有活跃玩家都需要行动，逐个过牌后本轮结束
TEST(BettingRoundTest, PostflopCheckAroundEndsFlow) {
    RoundRig rig;
    rig.setup(3, 0); // 按钮下一家先行动 = 1

    rig.betting_round.setupToStart(Stage::FLOP);
    EXPECT_EQ(1, rig.betting_round.getActingPlayer());
    EXPECT_FALSE(rig.betting_round.isFlowEnded()); // round_bet 为 0 也要行动
    EXPECT_EQ(3, rig.betting_round.needsToActCount());

    const Event::PlayerAction check = {.target_bet = 0, .is_fold = false, .is_all_in = false};
    for (int acted = 0; acted < 3; ++acted) {
        EXPECT_TRUE(rig.betting_round.applyAction(check).has_value());
        if (rig.betting_round.isFlowEnded()) break;
        rig.betting_round.moveActingPlayer();
    }
    EXPECT_EQ(0, rig.betting_round.needsToActCount());
    EXPECT_TRUE(rig.betting_round.isFlowEnded());
}

// 加注后，已行动但未跟平的玩家需要再次行动
TEST(BettingRoundTest, RaiseReopensForOthers) {
    RoundRig rig;
    rig.setup(3, 0); // SB=1 BB=2 UTG=0
    rig.betting_round.setupToStart(Stage::PREFLOP);
    rig.betting_round.setRoundBet(10); // 代表大盲 bring-in
    EXPECT_EQ(0, rig.betting_round.getActingPlayer());

    const Event::PlayerAction call = {.target_bet = 30, .is_fold = false, .is_all_in = false};
    EXPECT_TRUE(rig.betting_round.applyAction(call).has_value()); // 0 加注至 30
    EXPECT_EQ(30, rig.betting_round.getRoundBet());
    EXPECT_EQ(2, rig.betting_round.needsToActCount());

    rig.betting_round.moveActingPlayer();
    EXPECT_EQ(1, rig.betting_round.getActingPlayer());
    EXPECT_TRUE(rig.betting_round.applyAction(call).has_value());

    rig.betting_round.moveActingPlayer();
    EXPECT_EQ(2, rig.betting_round.getActingPlayer());
    EXPECT_TRUE(rig.betting_round.applyAction(call).has_value());

    EXPECT_EQ(0, rig.betting_round.needsToActCount());
    EXPECT_TRUE(rig.betting_round.isFlowEnded());
}

// 只剩一名在局玩家时，即使还有人未跟平，本轮也结束
TEST(BettingRoundTest, FlowEndsWithSinglePlayerAlive) {
    RoundRig rig;
    rig.setup(3, 0);
    rig.betting_round.setupToStart(Stage::PREFLOP);

    rig.player_set.playerAt(1).fold();
    rig.player_set.playerAt(2).fold();

    EXPECT_TRUE(rig.betting_round.isFlowEnded());
}

// ===== 处理玩家行动 =====

// 全下行动：返回的 PlayerActed 标记 is_all_in，且玩家状态变为 ALL_IN
TEST(BettingRoundTest, ApplyActionReportsAllIn) {
    RoundRig rig;
    rig.setup({1000, 100, 1000}, 0); // seat1 只有 100
    rig.betting_round.setupToStart(Stage::PREFLOP);
    rig.betting_round.setRoundBet(10);
    rig.betting_round.setActingPlayer(1);

    auto acted = rig.betting_round.applyAction(
        Event::PlayerAction{.target_bet = 100, .is_fold = false, .is_all_in = false});

    ASSERT_TRUE(acted.has_value());
    EXPECT_TRUE(acted->is_all_in);
    EXPECT_TRUE(rig.player_set.playerAt(1).isAllIn());
}

// 普通跟注：is_all_in 为 false
TEST(BettingRoundTest, ApplyActionCallIsNotAllIn) {
    RoundRig rig;
    rig.setup({1000, 1000, 1000}, 0);
    rig.betting_round.setupToStart(Stage::PREFLOP);
    rig.betting_round.setRoundBet(10);
    rig.betting_round.setActingPlayer(1);

    auto acted = rig.betting_round.applyAction(
        Event::PlayerAction{.target_bet = 10, .is_fold = false, .is_all_in = false});

    ASSERT_TRUE(acted.has_value());
    EXPECT_FALSE(acted->is_all_in);
    EXPECT_EQ(Action::Call, acted->action);
}

// 无效目标：返回 nullopt，不改变下注额
TEST(BettingRoundTest, ApplyActionRejectsInvalidTarget) {
    RoundRig rig;
    rig.setup(3, 0);
    rig.betting_round.setupToStart(Stage::PREFLOP);
    rig.betting_round.setRoundBet(10);
    rig.betting_round.setActingPlayer(1);

    auto acted = rig.betting_round.applyAction(
        Event::PlayerAction{.target_bet = 1, .is_fold = false, .is_all_in = false});

    EXPECT_FALSE(acted.has_value());
    EXPECT_EQ(0, rig.betting_round.getPlayerBetAt(1));
}
