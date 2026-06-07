#pragma once

#include <memory>
#include <vector>

#include "chips_calc.h"
#include "GameTypes.h"

namespace poker {
class SeatingManager;
class PotManager;
class RoundTracker;

/**
 * @brief 下注轮领域模型
 */
class BettingRound {
private:
    std::unique_ptr<RoundTracker> round_tracker;

    std::vector<chips_t> player_bets;  // 玩家的本轮下注额
    chips_t round_bet = 0;             // 此轮目前的最大下注额
    chips_t min_raise_delta = 0;       // 当前的最小加注额
    seat_idx_t actor_seat{};           // 当前行动玩家的位置
public:
    BettingRound();
    ~BettingRound();
    chips_t getPlayerBetAtSeat(seat_idx_t seat) const { return player_bets[seat]; }
    chips_t getRoundBet() const { return round_bet; }
    chips_t getMinRaiseDelta() const { return min_raise_delta; }
    seat_idx_t getActorSeat() const { return actor_seat; }
    void onGameStarted(int player_count);
    void onRoundStarted(seat_idx_t start_position);
    math::BettingContext getBettingContext(chips_t actor_chips) const;
    void applyBettingResult(const math::BettingResult& result);
    void advanceToNextActor(seat_idx_t next_actor_seat);
    bool isRoundFinished() const;
    bool isOnlyOnePlayerActive() const;
};
} // namespace poker