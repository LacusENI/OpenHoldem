#pragma once

#include <memory>
#include <vector>

#include "GameTypes.h"

namespace holdem {
class PlayerSet;
class PotManager;

enum class RoundState {
    IDLE, IN_PROGRESS, COMPLETED, ONLY_ONE_ACTIVE, ONLY_ONE_LIVE
};

class BettingRound {
private:
    RoundState round_state = RoundState::IDLE;
    std::shared_ptr<PlayerSet> player_set;
    std::shared_ptr<PotManager> pot_manager;

    std::vector<Stack> player_bets;  // 玩家的本轮下注额
    Stack round_bet = 0;             // 此轮目前的最大下注额
    Stack min_raise = 0;             // 当前的最小加注额

    Position current_position{};     // 当前行动玩家的位置
    int pending_players_count{};     // 尚未就当前最大下注额表态的玩家数
    Position last_raiser_position{}; // 最新的加注玩家的位置
    int active_players_count{};      // 剩余活跃(未弃牌)玩家数量
    int players_can_act_count{};     // 剩余可以行动的玩家数量
    Action processAction(const Action& action) const;
    void updateState(Position position, bool is_raise, bool is_fold);
    void nextTurn();
public:
    BettingRound(
        std::shared_ptr<PlayerSet> players, std::shared_ptr<PotManager> pot_manager);
    ~BettingRound();
    Position getCurrentPosition() const { return current_position; }
    bool isRoundEnded() const;
    void setup();
    void prepare(Position start_position);
    Action handleAction(const Action& action);
    void commitChips(Position position, Stack amount);
};
} // namespace holdem