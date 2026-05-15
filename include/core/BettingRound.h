#pragma once

#include <memory>
#include <vector>

#include "GameTypes.h"

namespace holdem {
class PlayerSet;
class PotManager;
class RoundTracker;

class BettingRound {
private:
    std::shared_ptr<PlayerSet> player_set;
    std::shared_ptr<PotManager> pot_manager;
    std::unique_ptr<RoundTracker> round_tracker;

    std::vector<Stack> player_bets;  // 玩家的本轮下注额
    Stack round_bet = 0;             // 此轮目前的最大下注额
    Stack min_raise = 0;             // 当前的最小加注额

    Position current_position{};     // 当前行动玩家的位置
    Action processAction(const Action& action) const;
    void nextTurn();
public:
    BettingRound(
        std::shared_ptr<PlayerSet> players, std::shared_ptr<PotManager> pot_manager);
    ~BettingRound();
    Position getCurrentPosition() const { return current_position; }
    bool isRoundEnded() const;
    void onGameStarted();
    void onRoundStarted(Position start_position);
    Action handleAction(const Action& action);
    void commitChips(Position position, Stack amount);
    Stack getChipsToCall() const;
    Stack getChipsToMinRaise() const;
};
} // namespace holdem