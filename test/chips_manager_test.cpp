#include <gtest/gtest.h>

#include <vector>

#include "ChipsManager.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "PotManager.h"
#include "SeatManager.h"

using namespace poker;

// 测试转移筹码
TEST(ChipsManagerTest, MoveChips) {
    PlayerSet player_set;
    PotManager pot_manager;
    ChipsManager chips_manager(&player_set, &pot_manager, nullptr);
    player_set.resize(5);
    pot_manager.addChips(100);
    chips_manager.setPlayerChipsAt(0, 20);
    chips_manager.setPlayerChipsAt(1, 50);
    // 底池 100 玩家 [0]: 20 [1]: 50

    // 从底池中拿出20筹码移至玩家0
    chips_manager.moveChipsToPlayer(0, 20);
    // 底池 80 玩家 [0]: 40 [1]: 50
    EXPECT_EQ(80, pot_manager.getPot());
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(0));
    EXPECT_EQ(50, chips_manager.getPlayerChipsAt(1));

    // 从玩家1手中拿出10筹码给到底池
    chips_manager.moveChipsToPot(1, 10);
    // 底池 90 玩家 [0]: 40 [1]: 40
    EXPECT_EQ(90, pot_manager.getPot());
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(0));
    EXPECT_EQ(40, chips_manager.getPlayerChipsAt(1));
}

// 测试分配底池计算
TEST(ChipsManagerTest, DistributePot) {
    PlayerSet player_set;
    PotManager pot_manager;
    SeatManager seat_manager(&player_set);
    ChipsManager chips_manager(&player_set, &pot_manager, &seat_manager);
    player_set.resize(9);
    for (Player& player : player_set) {
        player.setChips(0);
    }
    pot_manager.addChips(100);
    seat_manager.setBtnSeat(2);
    std::vector<seat_t> winners = {1, 2, 3, 4, 6, 7, 8};
    // 底池 100 赢家 7名 3, 4, 6, 7, 8, 1, 2

    chips_manager.distributePot(winners);
    // 获取筹码 [3]:15 [4]:15 [6]:14 [7]:14 [8]:14 [1]:14 [2]:14
    EXPECT_EQ(15, chips_manager.getPlayerChipsAt(3));
    EXPECT_EQ(15, chips_manager.getPlayerChipsAt(4));
    EXPECT_EQ(14, chips_manager.getPlayerChipsAt(6));
    EXPECT_EQ(0, chips_manager.getPlayerChipsAt(5));
    EXPECT_EQ(0, pot_manager.getPot());
}