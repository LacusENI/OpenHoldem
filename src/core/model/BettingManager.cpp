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
std::optional<UncalledBet> BettingManager::findUncalledBet() const {
    chips_t max1 = 0;  // 最高注
    chips_t max2 = 0;  // 次高注（严格小于 max1）
    int count_max = 0; // 达到 max1 的人数
    seat_t leader = INVALID_SEAT;

    for (seat_t seat = 0; seat < player_set->playerCount(); ++seat) {
        const Player& player = player_set->playerAt(seat);
        if (player.isFolded() || player.isOut()) continue; // 弃牌/出局者不计
        const chips_t bet = player.getBet();
        if (bet > max1) {
            max2 = max1;
            max1 = bet;
            count_max = 1;
            leader = seat;
        } else if (bet == max1) {
            ++count_max;
        } else if (bet > max2) {
            max2 = bet;
        }
    }

    if (leader != INVALID_SEAT && count_max == 1 && max1 > max2) {
        return UncalledBet{leader, max1 - max2};
    }
    return std::nullopt;
}
} // namespace poker
