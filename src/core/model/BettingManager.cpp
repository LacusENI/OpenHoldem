#include "BettingManager.h"

#include "GameTypes.hpp"
#include "PlayerSet.h"

namespace poker {
chips_t BettingManager::getPlayerBetAt(seat_idx_t seat_idx) const {
    return player_set->playerAt(seat_idx).bet;
}

void BettingManager::setPlayerBetAt(seat_idx_t seat_idx, chips_t player_bet) {
    player_set->playerAt(seat_idx).bet = player_bet;
}

} // namespace poker