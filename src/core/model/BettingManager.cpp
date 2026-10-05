#include "BettingManager.h"

#include <algorithm>

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
chips_t BettingManager::applySmallBlind(seat_t sb_seat_idx) {
    chips_t actual = std::min(getSmallBlind(), player_set->playerAt(sb_seat_idx).getChips());
    this->setPlayerBetAt(sb_seat_idx, actual);
    this->setRoundBet(std::max(getRoundBet(), actual));
    this->setMinRaiseDelta(std::max(getMinRaiseDelta(), actual));
    return actual;
}
chips_t BettingManager::applyBigBlind(seat_t bb_seat_idx) {
    chips_t actual = std::min(getBigBlind(), player_set->playerAt(bb_seat_idx).getChips());
    this->setPlayerBetAt(bb_seat_idx, actual);
    // 大盲名义额作为本轮的 bring-in，即使大盲玩家筹码不足
    this->setRoundBet(std::max(getRoundBet(), getBigBlind()));
    this->setMinRaiseDelta(std::max(getMinRaiseDelta(), getBigBlind()));
    return actual;
}
} // namespace poker
