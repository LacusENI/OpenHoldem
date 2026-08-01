#include "ActionFlow.h"

#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
void ActionFlow::moveToNextActor() {
    actor_seat = seat_manager->nextActiveSeat(actor_seat);
}
void ActionFlow::setupToStart(Stage stage) {
    switch (stage) {
    case Stage::PREFLOP: actor_seat = seat_manager->getUtgSeat(); break;
    case Stage::FLOP:
    case Stage::TURN:
    case Stage::RIVER: actor_seat = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
    default: break;
    }
    pending_player_count = player_set->getActivePlayerCount();
}
void ActionFlow::handlePlayerAction(Action action) {
    if (action == Action::RAISE) {
        pending_player_count = player_set->getActivePlayerCount() - 1;
    } else {
        pending_player_count--;
    }
}
bool ActionFlow::isEnded() {
    return pending_player_count == 0 || player_set->getAlivePlayerCount() == 1;
}
} // namespace poker