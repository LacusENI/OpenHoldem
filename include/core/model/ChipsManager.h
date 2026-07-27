#pragma once

#include "GameTypes.hpp"
#include "PlayerSet.h"
#include "PotManager.h"
namespace poker {
class ChipsManager {
private:
    PotManager* pot_manager;
    PlayerSet* player_set;

public:
    ChipsManager(PotManager* pot_manager, PlayerSet* player_set)
        : pot_manager(pot_manager), player_set(player_set) {}
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
};
} // namespace poker