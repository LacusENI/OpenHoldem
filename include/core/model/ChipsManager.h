#pragma once

#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.h"
#include "PotManager.h"
#include "chips_calc.h"

namespace poker {
class ChipsManager {
private:
    PlayerSet* player_set;
    PotManager* pot_manager;

public:
    ChipsManager(PlayerSet* player_set, PotManager* pot_manager)
        : player_set(player_set), pot_manager(pot_manager) {}
    ~ChipsManager() = default;

    chips_t getPlayerChipsAt(seat_idx_t seat_idx) { return player_set->playerAt(seat_idx).chips; }
    void setPlayerChipsAt(seat_idx_t seat_idx, chips_t player_chips) {
        player_set->playerAt(seat_idx).chips = player_chips;
    }
    void moveChipsToPot(seat_idx_t player_seat, chips_t chips) {
        player_set->playerAt(player_seat).bet(chips);
        pot_manager->addChips(chips);
    }
    void moveChipsToPlayer(seat_idx_t player_seat, chips_t chips) {
        pot_manager->takeChips(chips);
        player_set->playerAt(player_seat).win(chips);
    }
    /*@note winners 需要按照从庄家位左手侧开始的顺序排序 */
    void distributePot(std::vector<seat_idx_t> winners) {
        int winners_n = static_cast<int>(winners.size());
        auto awards = algo::calculate_awards(pot_manager->getPot(), winners_n);
        for (int i = 0; i < winners_n; i++) {
            seat_idx_t winner_seat = winners[i];
            chips_t award = awards[i];
            moveChipsToPlayer(winner_seat, award);
        }
    }
};
} // namespace poker