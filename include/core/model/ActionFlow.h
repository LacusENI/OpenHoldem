#pragma once

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class SeatManager;

class ActionFlow {
private:
    PlayerSet* player_set;
    SeatManager* seat_manager;
    seat_t acting_player;
    int pending_player_count = 0;

public:
    ActionFlow(PlayerSet* player_set, SeatManager* seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~ActionFlow() = default;

    seat_t getActingPlayer() const { return acting_player; }
    void setActingPlayer(seat_t player) { acting_player = player; }
    int getPendingPlayerCount() const { return pending_player_count; }

    void moveActingPlayer();
    void setupToStart(Stage stage);
    void applyAction(Action action);
    bool isFlowEnded();
};
} // namespace poker