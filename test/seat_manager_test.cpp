#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.h"
#include "SeatManager.h"

using namespace poker;

TEST(SeatManagerTest, SortSeatByDistFromBtnLeft) {
    PlayerSet player_set;
    player_set.resize(7);

    SeatManager seat_manager(&player_set);
    seat_manager.setBtnSeat(3);

    seat_idx_t ref_seat = seat_manager.nextSeat(seat_manager.getBtnSeat());
    std::vector<seat_idx_t> seats = {0, 1, 2, 3, 4, 5, 6};
    std::ranges::sort(seats, [&ref_seat, &seat_manager](seat_idx_t a, seat_idx_t b) {
        return seat_manager.distanceFromSeatLeft(a, ref_seat) <
               seat_manager.distanceFromSeatLeft(b, ref_seat);
    });

    std::vector<seat_idx_t> expected = {4, 5, 6, 0, 1, 2, 3};
    EXPECT_EQ(expected, seats);
}