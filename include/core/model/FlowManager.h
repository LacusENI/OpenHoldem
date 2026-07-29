#pragma once

#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class SeatManager;

class FlowManager {
private:
    PlayerSet* player_set;
    SeatManager* seat_manager;
    seat_idx_t actor_seat;
    int pending_player_count;

public:
    FlowManager(PlayerSet* player_set, SeatManager* seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~FlowManager() = default;

    seat_idx_t getActorSeat();
    void moveToNextActor();
    void setupFirstActor(Stage stage);
    bool isFlowEnded();
    void handlePlayerAction(Action action);
};
} // namespace poker