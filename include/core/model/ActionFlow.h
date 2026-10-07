#pragma once

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class SeatManager;

class ActionFlow {
private:
    PlayerSet* player_set;
    SeatManager* seat_manager;

    seat_t acting_player = INVALID_SEAT;
    seat_t last_aggressor = INVALID_SEAT;
    bool has_incomplete_raise = false;

public:
    ActionFlow(PlayerSet* player_set, SeatManager* seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~ActionFlow() = default;

    seat_t getActingPlayer() const { return acting_player; }
    void setActingPlayer(seat_t player) { acting_player = player; }
    /* 当前是否存在有效的行动者 */
    [[nodiscard]] bool hasActingPlayer() const { return acting_player != INVALID_SEAT; }
    /* 本轮最后一个激进者（下注/加注者）；无则 INVALID_SEAT */
    [[nodiscard]] seat_t getLastAggressor() const { return last_aggressor; }
    /* 当前仍需行动的玩家数量 */
    [[nodiscard]] int needsToActCount(chips_t round_bet) const;
    bool hasIncompleteRaise() const { return has_incomplete_raise; }

    bool canPlayerRaise(seat_t seat) const;
    void moveActingPlayer(chips_t round_bet);
    void setupToStart(Stage stage);
    void applyAction(Action action);
    /* 下注轮是否结束（无人需要行动，或仅剩一名在局玩家） */
    [[nodiscard]] bool isFlowEnded(chips_t round_bet) const;
};
} // namespace poker