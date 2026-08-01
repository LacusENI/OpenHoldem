#include "BettingManager.h"

#include "GameTypes.hpp"
#include "PlayerSet.hpp"

namespace poker {
chips_t BettingManager::getPlayerBetAt(seat_idx_t seat_idx) const {
    return player_set->playerAt(seat_idx).getBet();
}

void BettingManager::setPlayerBetAt(seat_idx_t seat_idx, chips_t player_bet) {
    player_set->playerAt(seat_idx).setBet(player_bet);
}

} // namespace poker