#include "RoundTracker.h"

namespace poker {
void RoundTracker::onGameStarted(int players_count) {
    active_players_count = players_count;
    players_can_act_count = players_count;
}

void RoundTracker::onRoundStarted() {
    pending_players_count = players_can_act_count;
}

void RoundTracker::onPlayerActed(Action action) {
    if (action == Action::RAISE) {
        pending_players_count = players_can_act_count;
    } else {
        pending_players_count--;
    }
    if (action == Action::FOLD) active_players_count--;
    if (action == Action::FOLD || action == Action::ALL_IN) players_can_act_count--;
}

bool RoundTracker::isRoundFinished() const {
    return (active_players_count == 1 || players_can_act_count == 1 || pending_players_count == 0);
}

bool RoundTracker::isOnlyOnePlayerActive() const {
    return active_players_count == 1;
}
} // holdem