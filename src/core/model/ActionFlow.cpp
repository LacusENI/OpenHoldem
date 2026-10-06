#include "ActionFlow.h"

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
void ActionFlow::moveActingPlayer(chips_t round_bet) {
    acting_player = seat_manager->nextSeatNeedsToAct(acting_player, round_bet);
}
int ActionFlow::needsToActCount(chips_t round_bet) const {
    int count = 0;
    for (const Player& player : *player_set) {
        if (player.needsToAct(round_bet)) ++count;
    }
    return count;
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
    for (Player& player : *player_set) {
        player.setNotActed();
    }
    last_aggressor = INVALID_SEAT;
    has_incomplete_raise = false;
}
void ActionFlow::applyAction(Action action) {
    if (action == Action::RAISE || action == Action::BET || action == Action::ALL_IN_RAISE ||
        action == Action::ALL_IN_UNDER_RAISE) {
        last_aggressor = acting_player;
        has_incomplete_raise = false;
    }
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
bool ActionFlow::isFlowEnded(chips_t round_bet) const {
    return needsToActCount(round_bet) == 0 || player_set->getAlivePlayerCount() == 1;
}
bool ActionFlow::canPlayerRaise(seat_t seat) const {
    const Player& player = player_set->playerAt(seat);
    return !(player.isActed() && has_incomplete_raise);
}
} // namespace poker
