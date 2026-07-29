#pragma once

#include <vector>

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class PotManager;
class SeatManager;

class ChipsManager {
private:
    PlayerSet* player_set;
    PotManager* pot_manager;
    SeatManager* seat_manager;

    void sortWinners(std::vector<seat_idx_t>& winners);

public:
    ChipsManager(PlayerSet* player_set, PotManager* pot_manager, SeatManager* seat_manager)
        : player_set(player_set), pot_manager(pot_manager), seat_manager(seat_manager) {}
    ~ChipsManager() = default;

    chips_t getPlayerChipsAt(seat_idx_t seat_idx);
    void setPlayerChipsAt(seat_idx_t seat_idx, chips_t player_chips);
    void moveChipsToPot(seat_idx_t player_seat, chips_t chips);
    void moveChipsToPlayer(seat_idx_t player_seat, chips_t chips);
    void awardWinners(const std::vector<seat_idx_t>& winners);
    void distributePot(std::vector<seat_idx_t> winners) {
        sortWinners(winners);
        awardWinners(winners);
    }
};
} // namespace poker