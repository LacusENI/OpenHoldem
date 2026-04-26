#pragma once

#include <memory>
#include <vector>

#include "GameTypes.h"

namespace holdem {
class PlayerSet;

enum class RoundState {
    IDLE, IN_PROGRESS, COMPLETED, ONLY_ONE_ACTIVE, ONLY_ONE_LIVE
};

class BettingRound {
private:
    RoundState round_state = RoundState::IDLE;
    std::shared_ptr<PlayerSet> players;

    std::vector<Stack> player_bets;  // 玩家的本轮下注额
    Stack round_bet = 0;             // 此轮目前的最大下注额
    Stack min_raise = 0;             // 当前的最小加注额

    Position rest_position{};        // 休止符玩家位置(若该玩家行动后下一位是休止符玩家，本轮下注结束)
    bool is_round_ended = false;     // 下注轮是否已结束

    Position current_position{};     // 当前行动玩家的位置
    int pending_players_count{};     // 尚未就当前最大下注额表态的玩家数
    Position last_raiser_position{}; // 最新的加注玩家的位置
    int active_players_count{};      // 剩余活跃(未弃牌)玩家数量
    int players_can_act_count{};     // 剩余可以行动的玩家数量
public:
    explicit BettingRound(std::shared_ptr<PlayerSet> players);
    Position getCurrentPosition() const { return current_position; }
    bool isRoundEnded() const;
    void prepare(Position start_position);
    Action handleAction(Action action, Stack big_blind) const;
    void nextTurn();
    void commitChips(Position position, Stack amount);
};
} // namespace holdem