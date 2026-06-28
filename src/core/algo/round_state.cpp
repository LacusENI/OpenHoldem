#include "round_state.h"

namespace poker::algo {

void setupGameStart(RoundState& round_state, int player_count) {
    round_state.players_can_act_count = player_count;
    round_state.players_can_act_count = player_count;
}

void setupRoundStart(RoundState& round_state) {
    round_state.pending_players_count = round_state.players_can_act_count;
}

void handlePlayerAction(RoundState& round_state, Action action) {
    if (action == Action::RAISE) {
        round_state.pending_players_count = round_state.players_can_act_count;
    } else {
        round_state.pending_players_count--;
    }
    if (action == Action::FOLD) {
        round_state.active_players_count--;
    }
    if (action == Action::FOLD || action == Action::ALL_IN) {
        round_state.players_can_act_count--;
    }
}

bool isRoundFinished(const RoundState& round_state) {
    return round_state.active_players_count == 1 ||
        round_state.players_can_act_count == 1 ||
            round_state.pending_players_count == 0;
}

bool isOnlyOnePlayerActive(const RoundState& round_state) {
    return round_state.active_players_count == 1;
}

} // namespace poker