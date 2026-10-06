#include "ActionFlow.h"

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
void ActionFlow::moveActingPlayer() {
    acting_player = seat_manager->nextActiveSeat(acting_player);
}
void ActionFlow::setupToStart(Stage stage) {
    if (isPreflop(stage)) {
        acting_player = seat_manager->getUtgSeat();
    } else if (isPostflop(stage)) {
        acting_player = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
    } else {
        // 非下注阶段（IDLE/SHOWDOWN/SETTLEMENT）没有行动者
        acting_player = INVALID_SEAT;
    }
    pending_player_count = player_set->getActivePlayerCount();
    for (Player& player : *player_set) {
        player.setNotActed();
    }
    last_aggressor = INVALID_SEAT;
    has_incomplete_raise = false;
}
void ActionFlow::applyAction(Action action) {
    if (action == Action::RAISE || action == Action::BET || action == Action::ALL_IN_RAISE ||
        action == Action::ALL_IN_UNDER_RAISE) {
        pending_player_count = player_set->getActivePlayerCount();
        last_aggressor = acting_player;
        has_incomplete_raise = false;
    }
    pending_player_count--;
    player_set->playerAt(acting_player).setActed();
    if (action == Action::FOLD) {
        player_set->playerAt(acting_player).fold();
    } else if (action == Action::ALL_IN_CALL || action == Action::ALL_IN_RAISE) {
        player_set->playerAt(acting_player).allIn();
    } else if (action == Action::ALL_IN_UNDER_CALL || action == Action::ALL_IN_UNDER_RAISE) {
        player_set->playerAt(acting_player).allIn();
    }
    if (action == Action::ALL_IN_UNDER_RAISE) {
        has_incomplete_raise = true;
    }
}
bool ActionFlow::isFlowEnded() {
    return pending_player_count == 0 || player_set->getAlivePlayerCount() == 1;
}
bool ActionFlow::canPlayerRaise(seat_t seat) const {
    const Player& player = player_set->playerAt(seat);
    return !(player.isActed() && has_incomplete_raise);
}
} // namespace poker