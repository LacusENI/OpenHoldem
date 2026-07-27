#pragma once

#include "GameTypes.hpp"

namespace poker {
class BettingManager {
private:
    chips_t big_blind = 10;
    chips_t round_bet = 0;
    chips_t min_raise_delta = 0;

public:
    BettingManager() = default;
    ~BettingManager() = default;

    chips_t getBigBlind() const { return big_blind; }
    chips_t getRoundBet() const { return round_bet; }
    chips_t getMinRaiseDelta() const { return min_raise_delta; }
    void setBigBlind(chips_t big_blind_) { big_blind = big_blind_; }
    void setRoundBet(chips_t round_bet_) { round_bet = round_bet_; }
    void setMinRaiseDelta(chips_t min_raise_delta_) { min_raise_delta = min_raise_delta_; }
};
} // namespace poker