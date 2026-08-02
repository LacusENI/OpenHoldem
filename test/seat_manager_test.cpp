#include <gtest/gtest.h>

#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

TEST(SeatManagerTest, SortSeats) {
    PlayerSet player_set;
    player_set.resize(7);

    SeatManager seat_manager(&player_set);
    seat_manager.setBtnSeat(3);

    std::vector<seat_idx_t> seats = {0, 1, 2, 3, 4, 5, 6};
    seat_manager.sortSeats(seats);

    std::vector<seat_idx_t> expected = {4, 5, 6, 0, 1, 2, 3};
    EXPECT_EQ(expected, seats);
}