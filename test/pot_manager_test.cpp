#include <gtest/gtest.h>

#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "PotManager.h"
#include "SeatManager.h"

using namespace poker;

// 测试分配底池计算
TEST(PotManagerTest, DistributePot) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    PotManager pot_manager(player_set, seat_manager);
    player_set.resize(9);
    for (Player& player : player_set) {
        player.setChips(0);
    }
    seat_manager.setBtnSeat(2);
    std::vector<seat_t> winners = {1, 2, 3, 4, 6, 7, 8};
    // 底池 100，7 名赢家，排序后为 3,4,6,7,8,1,2，余数 2 给前两名
    pot_manager.distributePot(100, winners);
    // 获取筹码 [3]:15 [4]:15 [6]:14 [7]:14 [8]:14 [1]:14 [2]:14
    EXPECT_EQ(15, pot_manager.getPlayerChipsAt(3));
    EXPECT_EQ(15, pot_manager.getPlayerChipsAt(4));
    EXPECT_EQ(14, pot_manager.getPlayerChipsAt(6));
    EXPECT_EQ(0, pot_manager.getPlayerChipsAt(5));
}

// 空赢家分配应安全返回，不改变任何筹码（P1-2 回归）
TEST(PotManagerTest, DistributePotWithNoWinners) {
    PlayerSet player_set;
    SeatManager seat_manager(player_set);
    PotManager pot_manager(player_set, seat_manager);
    player_set.resize(3);
    for (Player& player : player_set) {
        player.setChips(0);
    }

    std::vector<seat_t> winners;
    EXPECT_NO_THROW(pot_manager.distributePot(100, winners));
    for (int seat = 0; seat < player_set.playerCount(); ++seat) {
        EXPECT_EQ(0, pot_manager.getPlayerChipsAt(seat));
    }
}
