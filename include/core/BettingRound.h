#pragma once

#include <memory>
#include <vector>

#include "GameTypes.h"

namespace holdem {
class PlayerSet;

class BettingRound {
private:
    std::shared_ptr<PlayerSet> players;

    Stack round_bet = 0;             // 此轮目前的最大下注额
    Position rest_position{};        // 休止符玩家位置(若该玩家行动后下一位是休止符玩家，本轮下注结束)
    Position current_position{};     // 当前行动玩家的位置
    bool is_round_ended = false;     // 下注轮是否已结束
    std::vector<Stack> player_bets;  // 玩家的本轮下注额
public:
    explicit BettingRound(std::shared_ptr<PlayerSet> players);
    Position getCurrentPosition() const { return current_position; }
    bool isRoundEnded() const;
    void initialize(Position start_position);
    Action takeAction(Action action, Stack big_blind);
    void nextActor();
    void commitChips(Position position, Stack amount);
};
} // namespace holdem