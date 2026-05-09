#include "RoundTracker.h"

namespace holdem {
void RoundTracker::onGameStarted(int players_count) {
    active_players_count = players_count;
    players_can_act_count = players_count;
}

void RoundTracker::onRoundStarted() {
    pending_players_count = players_can_act_count;
}

void RoundTracker::onPlayerActed(ActionType action) {
    if (action == ActionType::RAISE) {
        pending_players_count = players_can_act_count;
    } else {
        pending_players_count--;
    }
    if (action == ActionType::FOLD) active_players_count--;
    if (action == ActionType::FOLD || action == ActionType::ALL_IN) players_can_act_count--;
}

bool RoundTracker::isRoundEnded() const {
    return (active_players_count == 1 || players_can_act_count == 1 || pending_players_count == 0);
}
} // holdem