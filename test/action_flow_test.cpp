#include <gtest/gtest.h>

#include "ActionFlow.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

namespace {
struct FlowRig {
    PlayerSet player_set;
    SeatManager seat_manager{&player_set};
    ActionFlow action_flow{&player_set, &seat_manager};

    void setup(int player_count, seat_t btn) {
        player_set.resize(player_count);
        for (int s = 0; s < player_set.playerCount(); ++s) {
            player_set.playerAt(s).setChips(1000);
        }
        seat_manager.setBtnSeat(btn);
    }
};
} // namespace

// 翻前从 UTG 开始行动（P2-1）
TEST(ActionFlowTest, PreflopStartsAtUtg) {
    FlowRig rig;
    rig.setup(4, 0); // btn=0, SB=1, BB=2, UTG=3
    rig.action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(3, rig.action_flow.getActingPlayer());
}

// 翻后从按钮左手第一家开始行动（P2-1）
TEST(ActionFlowTest, PostflopStartsLeftOfButton) {
    FlowRig rig;
    rig.setup(4, 0); // 按钮下一家 = 1
    for (Stage stage : {Stage::FLOP, Stage::TURN, Stage::RIVER}) {
        rig.action_flow.setupToStart(stage);
        EXPECT_EQ(1, rig.action_flow.getActingPlayer());
    }
}

// 非下注阶段没有有效行动者，且不会沿用旧值（P2-1）
TEST(ActionFlowTest, NonBettingStageClearsActingPlayer) {
    FlowRig rig;
    rig.setup(3, 0);

    rig.action_flow.setupToStart(Stage::PREFLOP);
    ASSERT_NE(INVALID_SEAT, rig.action_flow.getActingPlayer());

    for (Stage stage : {Stage::IDLE, Stage::SHOWDOWN, Stage::SETTLEMENT}) {
        rig.action_flow.setupToStart(stage);
        EXPECT_EQ(INVALID_SEAT, rig.action_flow.getActingPlayer());
        EXPECT_FALSE(rig.action_flow.hasActingPlayer());
    }
}

// 翻后无人下注时，所有活跃玩家都需要行动，逐个过牌后本轮结束（P2-2 重构）
TEST(ActionFlowTest, PostflopCheckAroundEndsFlow) {
    FlowRig rig;
    rig.setup(3, 0); // 按钮下一家先行动 = 1

    rig.action_flow.setupToStart(Stage::FLOP);
    EXPECT_EQ(1, rig.action_flow.getActingPlayer());
    EXPECT_FALSE(rig.action_flow.isFlowEnded(0)); // round_bet 为 0 也要行动
    EXPECT_EQ(3, rig.action_flow.needsToActCount(0));

    for (int acted = 0; acted < 3; ++acted) {
        rig.action_flow.applyAction(Action::CHECK);
        if (rig.action_flow.isFlowEnded(0)) break;
        rig.action_flow.moveActingPlayer(0);
    }
    EXPECT_EQ(0, rig.action_flow.needsToActCount(0));
    EXPECT_TRUE(rig.action_flow.isFlowEnded(0));
}

// 加注后，已行动但未跟平的玩家需要再次行动（P2-2 重构）
TEST(ActionFlowTest, RaiseReopensForOthers) {
    FlowRig rig;
    rig.setup(3, 0); // SB=1 BB=2 UTG=0
    rig.action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(0, rig.action_flow.getActingPlayer());

    const chips_t round_bet = 30;
    rig.player_set.playerAt(0).setBet(round_bet);
    rig.action_flow.applyAction(Action::RAISE);
    EXPECT_EQ(2, rig.action_flow.needsToActCount(round_bet));

    rig.action_flow.moveActingPlayer(round_bet);
    EXPECT_EQ(1, rig.action_flow.getActingPlayer());
    rig.player_set.playerAt(1).setBet(round_bet);
    rig.action_flow.applyAction(Action::CALL);

    rig.action_flow.moveActingPlayer(round_bet);
    EXPECT_EQ(2, rig.action_flow.getActingPlayer());
    rig.player_set.playerAt(2).setBet(round_bet);
    rig.action_flow.applyAction(Action::CALL);

    EXPECT_EQ(0, rig.action_flow.needsToActCount(round_bet));
    EXPECT_TRUE(rig.action_flow.isFlowEnded(round_bet));
}

// 只剩一名在局玩家时，即使还有人未跟平，本轮也结束（P2-2 重构）
TEST(ActionFlowTest, FlowEndsWithSinglePlayerAlive) {
    FlowRig rig;
    rig.setup(3, 0);
    rig.action_flow.setupToStart(Stage::PREFLOP);

    rig.player_set.playerAt(1).fold();
    rig.player_set.playerAt(2).fold();

    EXPECT_TRUE(rig.action_flow.isFlowEnded(0));
}
