#pragma once
#include "GameTypes.h"

namespace holdem {
/**
 * @brief 下注轮进度追踪器
 * @details 追踪下注轮的进度，判断轮次是否结束
 */
class RoundTracker {
private:
    int pending_players_count{};     // 尚未就当前最大下注额表态的玩家数
    int active_players_count{};      // 剩余活跃(未弃牌)玩家数量
    int players_can_act_count{};     // 剩余可以行动的玩家数量
public:
    RoundTracker() = default;
    void onGameStarted(int players_count);
    void onRoundStarted();
    void onPlayerActed(ActionType action);
    bool isRoundEnded() const;
};
} // holdem