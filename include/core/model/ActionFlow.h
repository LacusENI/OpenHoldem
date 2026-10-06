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
    int pending_player_count = 0;
    bool has_incomplete_raise = false;

public:
    ActionFlow(PlayerSet* player_set, SeatManager* seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~ActionFlow() = default;

    seat_t getActingPlayer() const { return acting_player; }
    void setActingPlayer(seat_t player) { acting_player = player; }
    /* 当前是否存在有效的行动者 */
    [[nodiscard]] bool hasActingPlayer() const { return acting_player != INVALID_SEAT; }
    int getPendingPlayerCount() const { return pending_player_count; }
    void setPendingPlayerCount(int count) { pending_player_count = count; }
    bool hasIncompleteRaise() const { return has_incomplete_raise; }

    bool canPlayerRaise(seat_t seat) const;
    void moveActingPlayer();
    void setupToStart(Stage stage);
    void applyAction(Action action);
    bool isFlowEnded();
};
} // namespace poker