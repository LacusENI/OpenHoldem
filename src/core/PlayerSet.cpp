#include "PlayerSet.h"

namespace poker {
void PlayerSet::addNewPlayer() {
    seat_idx_t position = players.size();
    players.emplace_back(position);
}

seat_idx_t PlayerSet::nextPosition(seat_idx_t position) const {
    return (position + 1) % static_cast<int>(players.size());
}

seat_idx_t PlayerSet::nextPositionToAct(seat_idx_t position) const {
    seat_idx_t next_pos = nextPosition(position);
    while (players[next_pos].is_folded && next_pos != position) {
        next_pos = nextPosition(next_pos);
    }
    return players[next_pos].is_folded ? -1 : next_pos;
}

seat_idx_t PlayerSet::getSbPosition(seat_idx_t btn_position) const {
    return nextPosition(btn_position);
}

seat_idx_t PlayerSet::getBbPosition(seat_idx_t btn_position) const {
    return nextPosition(nextPosition(btn_position));
}

seat_idx_t PlayerSet::getUtgPosition(seat_idx_t btn_position) const {
    return nextPosition(getBbPosition(btn_position));
}
} // namespace holdem