#pragma once
#include "GameTypes.h"

namespace poker::algo {
struct RoundState {
    int pending_players_count{};     // 尚未就当前最大下注额表态的玩家数
    int active_players_count{};      // 剩余活跃(未弃牌)玩家数量
    int players_can_act_count{};     // 剩余可以行动的玩家数量
};

void setupGameStart(RoundState& round_state, int player_count);

void setupRoundStart(RoundState& round_state);

void handlePlayerAction(RoundState& round_state, Action action);

bool isRoundFinished(const RoundState& round_state);

bool isOnlyOnePlayerActive(const RoundState& round_state);

} // namespace poker