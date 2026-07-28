#pragma once

#include <algorithm>
#include <vector>

#include "GameTypes.hpp"
#include "PlayerSet.h"
#include "PotManager.h"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
class ChipsManager {
private:
    PlayerSet* player_set;
    PotManager* pot_manager;
    SeatManager* seat_manager;

    void sortWinners(std::vector<seat_idx_t>& winners) {
        seat_idx_t ref_seat = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
        std::ranges::sort(winners, [&ref_seat, this](seat_idx_t seat_a, seat_idx_t seat_b) {
            return this->seat_manager->distanceFromSeatLeft(seat_a, ref_seat) <
                   this->seat_manager->distanceFromSeatLeft(seat_b, ref_seat);
        });
    }

public:
    ChipsManager(PlayerSet* player_set, PotManager* pot_manager, SeatManager* seat_manager)
        : player_set(player_set), pot_manager(pot_manager), seat_manager(seat_manager) {}
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
    void awardWinners(const std::vector<seat_idx_t>& winners) {
        int winners_n = static_cast<int>(winners.size());
        auto awards = algo::calculate_awards(pot_manager->getPot(), winners_n);
        for (int i = 0; i < winners_n; i++) {
            seat_idx_t winner_seat = winners[i];
            chips_t award = awards[i];
            moveChipsToPlayer(winner_seat, award);
        }
    }
    void distributePot(std::vector<seat_idx_t> winners) {
        sortWinners(winners);
        awardWinners(winners);
    }
};
} // namespace poker