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
