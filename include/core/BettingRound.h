#pragma once

#include <memory>
#include <vector>

#include "GameTypes.h"

namespace poker {
class SeatingManager;
class PotManager;
class RoundTracker;

class BettingRound {
private:
    std::shared_ptr<SeatingManager> seating_manager;
    std::shared_ptr<PotManager> pot_manager;
    std::unique_ptr<RoundTracker> round_tracker;

    std::vector<chips_t> player_bets;  // 玩家的本轮下注额
    chips_t round_bet = 0;             // 此轮目前的最大下注额
    chips_t min_raise = 0;             // 当前的最小加注额

    seat_idx_t current_position{};     // 当前行动玩家的位置
    Action processAction(const Action& action) const;
    void nextTurn();
public:
    BettingRound(
        std::shared_ptr<SeatingManager> seating_manager, std::shared_ptr<PotManager> pot_manager);
    ~BettingRound();
    seat_idx_t getCurrentPosition() const { return current_position; }
    bool isRoundEnded() const;
    void onGameStarted();
    void onRoundStarted(seat_idx_t start_position);
    Action handleAction(const Action& action);
    void commitChips(seat_idx_t position, chips_t amount);
    chips_t getChipsToCall() const;
    chips_t getChipsToMinRaise() const;
};
} // namespace holdem