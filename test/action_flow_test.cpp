#include <gtest/gtest.h>

#include "ActionFlow.h"
#include "GameTypes.hpp"
#include "SeatManager.h"

using namespace poker;

TEST(ActionFlowTest, MoveToNextActor) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    action_flow.setActingPlayer(0);
    action_flow.moveActingPlayer();
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());

    action_flow.setActingPlayer(4);
    action_flow.moveActingPlayer();
    EXPECT_EQ(0, action_flow.getActingPlayer());

    // 测试是否会跳过已弃牌玩家
    player_set.playerAt(1).fold();
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
}

TEST(ActionFlowTest, SetupToStart) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    seat_manager.setBtnSeat(3);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(1, action_flow.getActingPlayer());

    action_flow.setupToStart(Stage::FLOP);
    EXPECT_EQ(4, action_flow.getActingPlayer());

    player_set.playerAt(4).fold();
    action_flow.setupToStart(Stage::TURN);
    EXPECT_EQ(0, action_flow.getActingPlayer());
}

TEST(ActionFlowTest, FlowEndByAllCalled) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);

    // 从 1 开始, 一直跟注, 直到 0 过牌, 检验流程是否结束
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(4, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(0, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CHECK);
    EXPECT_TRUE(action_flow.isFlowEnded());
}

TEST(ActionFlowTest, OneRaiseThenAllCall) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);

    // 从 1 开始, 让 2 加注, 此后一直跟注, 检验 1 跟注之后流程是否结束
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::RAISE);
    action_flow.moveActingPlayer();
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(4, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(0, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    EXPECT_TRUE(action_flow.isFlowEnded());
}

TEST(ActionFlowTest, SkipFoldedPlayer) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    player_set.playerAt(1).fold();
    player_set.playerAt(4).fold();
    action_flow.setupToStart(Stage::FLOP);

    EXPECT_EQ(0, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CHECK);
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CHECK);
    action_flow.moveActingPlayer();
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CHECK);
    EXPECT_TRUE(action_flow.isFlowEnded());
}

TEST(ActionFlowTest, IgnoreFoldedPlayer) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);

    // 让 1 和 2 弃牌, 检验之后流程是否跳过 1 和 2
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    EXPECT_TRUE(player_set.playerAt(1).isFolded());
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    EXPECT_TRUE(player_set.playerAt(2).isFolded());
    action_flow.moveActingPlayer();
    // 3 加注, 由于 1 和 2 弃牌, 所以流程应该在 4, 0 跟注后结束
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::RAISE);
    action_flow.moveActingPlayer();
    EXPECT_EQ(4, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(0, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_TRUE(action_flow.isFlowEnded());
}

TEST(ActionFlowTest, FoldAfterRaise) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::RAISE);
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    action_flow.moveActingPlayer();
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    action_flow.moveActingPlayer();
    EXPECT_EQ(4, action_flow.getActingPlayer());
    action_flow.applyAction(Action::RAISE);
    action_flow.moveActingPlayer();
    EXPECT_EQ(0, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    action_flow.moveActingPlayer();
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::CALL);
    EXPECT_TRUE(action_flow.isFlowEnded());
}

TEST(ActionFlowTest, OnlyOneNotFolded) {
    PlayerSet player_set;
    SeatManager seat_manager(&player_set);
    ActionFlow action_flow(&player_set, &seat_manager);

    player_set.resize(5);
    // [3] BTN [4] SB [0] BB [1] UTG [2] CO
    seat_manager.setBtnSeat(3);
    action_flow.setupToStart(Stage::PREFLOP);
    EXPECT_EQ(1, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    action_flow.moveActingPlayer();
    EXPECT_EQ(2, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    action_flow.moveActingPlayer();
    EXPECT_EQ(3, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    action_flow.moveActingPlayer();
    EXPECT_EQ(4, action_flow.getActingPlayer());
    action_flow.applyAction(Action::FOLD);
    EXPECT_TRUE(action_flow.isFlowEnded());
}
