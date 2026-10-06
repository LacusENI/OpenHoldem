#pragma once

#include <optional>

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;

/* 未被跟注的多余下注：应退还给该玩家 */
struct UncalledBet {
    seat_t seat;
    chips_t amount;
};

class BettingManager {
private:
    PlayerSet* player_set;

    chips_t big_blind = 10;
    chips_t round_bet = 0;
    chips_t min_raise_delta = 0;

public:
    BettingManager(PlayerSet* player_set) : player_set(player_set) {}
    ~BettingManager() = default;

    chips_t getPlayerBetAt(seat_t seat_idx) const;
    chips_t getBigBlind() const { return big_blind; }
    chips_t getSmallBlind() const { return big_blind / 2; }
    chips_t getRoundBet() const { return round_bet; }
    chips_t getMinRaiseDelta() const { return min_raise_delta; }
    chips_t getMinRaise() const { return round_bet + min_raise_delta; }
    void setPlayerBetAt(seat_t seat_idx, chips_t player_bet);
    void setBigBlind(chips_t big_blind) { this->big_blind = big_blind; }
    void setRoundBet(chips_t round_bet) { this->round_bet = round_bet; }
    void setMinRaiseDelta(chips_t min_raise_delta) { this->min_raise_delta = min_raise_delta; }

    void setupBetting();
    /* 执行小盲注，返回实际投入量（筹码不足时投入全部筹码） */
    chips_t applySmallBlind(seat_t sb_seat_idx);
    /* 执行大盲注，返回实际投入量（筹码不足时投入全部筹码） */
    chips_t applyBigBlind(seat_t bb_seat_idx);

    /* 查找本轮唯一最高下注者未被跟注的部分；没有则返回 nullopt */
    std::optional<UncalledBet> findUncalledBet() const;
};
} // namespace poker