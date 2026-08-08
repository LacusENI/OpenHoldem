#include "ActionFlow.h"

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
void ActionFlow::moveActingPlayer() {
    acting_player = seat_manager->nextActiveSeat(acting_player);
}
void ActionFlow::setupToStart(Stage stage) {
    switch (stage) {
    case Stage::PREFLOP: acting_player = seat_manager->getUtgSeat(); break;
    case Stage::FLOP:
    case Stage::TURN:
    case Stage::RIVER: acting_player = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
    default: break;
    }
    pending_player_count = player_set->getActivePlayerCount();
}
void ActionFlow::applyAction(Action action) {
    if (action == Action::RAISE || action == Action::BET) {
        pending_player_count = player_set->getActivePlayerCount();
        last_aggressor = acting_player;
    }
    pending_player_count--;
    if (action == Action::FOLD) {
        player_set->playerAt(acting_player).fold();
    } else if (action == Action::ALL_IN) {
        player_set->playerAt(acting_player).allIn();
    }
}
bool ActionFlow::isFlowEnded() {
    return pending_player_count == 0 || player_set->getAlivePlayerCount() == 1;
}
} // namespace poker