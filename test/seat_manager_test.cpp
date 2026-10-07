#include <gtest/gtest.h>

#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

TEST(SeatManagerTest, SortSeats) {
    PlayerSet player_set;
    player_set.resize(7);

    SeatManager seat_manager(player_set);
    seat_manager.setBtnSeat(3);

    std::vector<seat_t> seats = {0, 1, 2, 3, 4, 5, 6};
    seat_manager.sortSeats(seats);

    std::vector<seat_t> expected = {4, 5, 6, 0, 1, 2, 3};
    EXPECT_EQ(expected, seats);
}

// 单挑：按钮位本人即小盲，另一人为大盲，翻前由按钮位先行动（P1-1 回归）
TEST(SeatManagerTest, HeadsUpBlindPositions) {
    PlayerSet player_set;
    player_set.resize(2);

    SeatManager seat_manager(player_set);
    seat_manager.setBtnSeat(0);

    EXPECT_EQ(0, seat_manager.getSbSeat());
    EXPECT_EQ(1, seat_manager.getBbSeat());
    EXPECT_EQ(0, seat_manager.getUtgSeat());
}

// 单挑且存在已出局座位时，仍需正确识别在局的两名玩家（P1-1 回归）
TEST(SeatManagerTest, HeadsUpWithEliminatedSeat) {
    PlayerSet player_set;
    player_set.resize(3);
    player_set.playerAt(1).out();

    SeatManager seat_manager(player_set);
    seat_manager.setBtnSeat(0);

    EXPECT_EQ(0, seat_manager.getSbSeat());
    EXPECT_EQ(2, seat_manager.getBbSeat());
    EXPECT_EQ(0, seat_manager.getUtgSeat());
}