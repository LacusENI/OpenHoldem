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
    std::shared_ptr<SeatingManager> seating_manager;
    std::shared_ptr<PotManager> pot_manager;
    std::unique_ptr<RoundTracker> round_tracker;

    std::vector<chips_t> player_bets;  // 玩家的本轮下注额
    chips_t round_bet = 0;             // 此轮目前的最大下注额
    chips_t min_raise_delta = 0;       // 当前的最小加注额
    seat_idx_t actor_seat{};           // 当前行动玩家的位置
public:
    BettingRound(
        std::shared_ptr<SeatingManager> seating_manager, std::shared_ptr<PotManager> pot_manager);
    ~BettingRound();
    chips_t getPlayerBetAt(seat_idx_t seat) const { return player_bets[seat]; }
    chips_t getRoundBet() const { return round_bet; }
    chips_t getMinRaiseDelta() const { return min_raise_delta; }
    seat_idx_t getActorSeat() const { return actor_seat; }
    void onGameStarted();
    void startBettingRound(seat_idx_t start_position);
    math::BettingContext getBettingContext() const;
    void processBettingInput(math::BettingInput betting_input);
    void advanceToNextActor();
    bool isRoundEnded() const;
    bool isOnlyOnePlayerActive() const;
    bool isNoPlayerNeedToAct() const;
};
} // namespace poker