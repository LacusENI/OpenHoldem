#pragma once

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class SeatManager;

class ActionFlow {
private:
    PlayerSet* player_set;
    SeatManager* seat_manager;
    seat_idx_t actor_seat;
    int pending_player_count;

public:
    ActionFlow(PlayerSet* player_set, SeatManager* seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~ActionFlow() = default;

    seat_idx_t getActorSeat() { return actor_seat; }
    void setActorSeat(seat_idx_t seat) { actor_seat = seat; }
    void moveToNextActor();
    void setupToStart(Stage stage);
    void handlePlayerAction(Action action);
    bool isEnded();
};
} // namespace poker