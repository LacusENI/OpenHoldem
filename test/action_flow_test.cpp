#include <gtest/gtest.h>

#include "ActionFlow.h"
#include "SeatManager.h"

using namespace poker;

TEST(ActionFlowTest, MoveToNextActor) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    action_flow.setActorSeat(0);
    action_flow.moveToNextActor();
    EXPECT_EQ(1, action_flow.getActorSeat());
    action_flow.moveToNextActor();
    EXPECT_EQ(2, action_flow.getActorSeat());
    action_flow.setActorSeat(4);
    action_flow.moveToNextActor();
    EXPECT_EQ(0, action_flow.getActorSeat());
}

TEST(ActionFlowTest, SetupToStart) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    seat_manager.setBtnSeat(3);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(1, action_flow.getActorSeat());

    action_flow.setupToStart(Stage::FLOP);
    EXPECT_EQ(4, action_flow.getActorSeat());

    player_set.playerAt(4).is_folded = true;
    action_flow.setupToStart(Stage::TURN);
    EXPECT_EQ(0, action_flow.getActorSeat());
}

TEST(ActionFlowTest, HandlePlayerAction) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);

    // 从 1 开始, 让 2 加注, 此后一直跟注, 检验 1 跟注之后流程是否结束
    EXPECT_EQ(1, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::CALL);
    action_flow.moveToNextActor();
    EXPECT_EQ(2, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::RAISE);
    action_flow.moveToNextActor();
    EXPECT_EQ(3, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::CALL);
    action_flow.moveToNextActor();
    EXPECT_EQ(4, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::CALL);
    action_flow.moveToNextActor();
    EXPECT_EQ(0, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::CALL);
    action_flow.moveToNextActor();
    EXPECT_EQ(1, action_flow.getActorSeat());
    action_flow.handlePlayerAction(Action::CALL);
    EXPECT_TRUE(action_flow.isEnded());
}