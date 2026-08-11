#include "side_pots.h"

#include <gtest/gtest.h>

#include <vector>

#include "GameTypes.hpp"

using namespace poker;

TEST(SidePotsTest, LoadSidePots) {
    std::vector<std::pair<seat_t, chips_t>> parameter = {
        {0, 120}, {1, 140}, {2, 140}, {3, 56}, {4, 72}, {5, 72}, {6, 140}, {7, 89},
    };
    // Pot [0]: 0,1,2,3,4,5,6,7 level=56  contribution=56 chips=448
    // Pot [1]: 0,1,2,  4,5,6,7 level=72  contribution=16 chips=112
    // Pot [2]: 0,1,2,      6,7 level=89  contribution=17 chips=85
    // Pot [3]: 0,1,2,      6   level=120 contribution=31 chips=124
    // Pot [4]:   1,2,      6   level=140 contribution=20 chips=60

    std::vector<Pot> pots = load_side_pots(parameter);
    EXPECT_EQ(5, pots.size());
    EXPECT_EQ(56, pots[0].level);
    EXPECT_EQ(72, pots[1].level);
    EXPECT_EQ(89, pots[2].level);
    EXPECT_EQ(120, pots[3].level);
    EXPECT_EQ(140, pots[4].level);

    chips_t total_committed = 0;
    for (auto [_, committed] : parameter) {
        total_committed += committed;
    }
    chips_t total_pot_chips = 0;
    for (const Pot& pot : pots) {
        total_pot_chips += pot.chips;
    }
    EXPECT_EQ(total_committed, total_pot_chips);

    chips_t total_contribution_p4 = pots[0].contribution + pots[1].contribution;
    chips_t p4_committed = parameter[4].second;
    EXPECT_EQ(p4_committed, total_contribution_p4);
}