#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "ChipsManager.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

// 测试转移筹码
TEST(ChipsManagerTest, MoveChips) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    ChipsManager chips_manager(player_set, seat_manager);
    player_set.resize(2);
    chips_manager.setPlayerChipsAt(0, 20);
    chips_manager.setPlayerChipsAt(1, 50);

    // 给玩家0发放20筹码
    chips_manager.moveChipsToPlayer(0, 20);
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(0));
    EXPECT_EQ(50, chips_manager.getPlayerChipsAt(1));

    // 玩家1向底池投入10筹码
    chips_manager.moveChipsToPot(1, 10);
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(0));
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(1));
    EXPECT_EQ(10, player_set.playerAt(1).getCommitted());
}

// 测试筹码不足时拒绝投入
TEST(ChipsManagerTest, MoveChipsNotEnough) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    ChipsManager chips_manager(player_set, seat_manager);
    player_set.resize(1);
    chips_manager.setPlayerChipsAt(0, 10);

    EXPECT_THROW(chips_manager.moveChipsToPot(0, 20), std::invalid_argument);
}

// 测试分配底池计算
TEST(ChipsManagerTest, DistributePot) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    ChipsManager chips_manager(player_set, seat_manager);
    player_set.resize(9);
    for (Player& player : player_set) {
        player.setChips(0);
    }
    seat_manager.setBtnSeat(2);
    std::vector<seat_t> winners = {1, 2, 3, 4, 6, 7, 8};
    // 底池 100，7 名赢家，排序后为 3,4,6,7,8,1,2，余数 2 给前两名
    chips_manager.distributePot(100, winners);
    // 获取筹码 [3]:15 [4]:15 [6]:14 [7]:14 [8]:14 [1]:14 [2]:14
    EXPECT_EQ(15, chips_manager.getPlayerChipsAt(3));
    EXPECT_EQ(15, chips_manager.getPlayerChipsAt(4));
    EXPECT_EQ(14, chips_manager.getPlayerChipsAt(6));
    EXPECT_EQ(0, chips_manager.getPlayerChipsAt(5));
}

// 空赢家分配应安全返回，不改变任何筹码（P1-2 回归）
TEST(ChipsManagerTest, DistributePotWithNoWinners) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    ChipsManager chips_manager(player_set, seat_manager);
    player_set.resize(3);
    for (Player& player : player_set) {
        player.setChips(0);
    }

    std::vector<seat_t> winners;
    EXPECT_NO_THROW(chips_manager.distributePot(100, winners));
    for (int seat = 0; seat < player_set.playerCount(); ++seat) {
        EXPECT_EQ(0, chips_manager.getPlayerChipsAt(seat));
    }
}
