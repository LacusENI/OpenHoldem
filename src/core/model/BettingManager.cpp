#include "BettingManager.h"

#include "GameTypes.hpp"
#include "PlayerSet.hpp"

namespace poker {
chips_t BettingManager::getPlayerBetAt(seat_t seat_idx) const {
    return player_set->playerAt(seat_idx).getBet();
}
void BettingManager::setPlayerBetAt(seat_t seat_idx, chips_t player_bet) {
    player_set->playerAt(seat_idx).setBet(player_bet);
}
void BettingManager::setupBetting() {
    this->setRoundBet(0);
    this->setMinRaiseDelta(0);
    for (seat_t seat_idx : player_set->getPlayerSeats()) {
        this->setPlayerBetAt(seat_idx, 0);
    }
}
void BettingManager::applySmallBlind(seat_t sb_seat_idx) {
    this->setPlayerBetAt(sb_seat_idx, getSmallBlind());
    this->setRoundBet(getSmallBlind());
    this->setMinRaiseDelta(getSmallBlind());
}
void BettingManager::applyBigBlind(seat_t bb_seat_idx) {
    this->setPlayerBetAt(bb_seat_idx, getBigBlind());
    this->setRoundBet(getBigBlind());
    this->setMinRaiseDelta(getBigBlind());
}
} // namespace poker