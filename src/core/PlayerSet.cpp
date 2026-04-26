#include "PlayerSet.h"

namespace holdem {
void PlayerSet::addNewPlayer() {
    Position position = players.size();
    players.emplace_back(position);
}

Position PlayerSet::nextPosition(Position position) const {
    return (position + 1) % static_cast<int>(players.size());
}

Position PlayerSet::nextPositionToAct(Position position) const {
    Position next_pos = nextPosition(position);
    while (players[next_pos].is_folded && next_pos != position) {
        next_pos = nextPosition(next_pos);
    }
    return players[next_pos].is_folded ? -1 : next_pos;
}

Position PlayerSet::getSbPosition(Position btn_position) const {
    return nextPosition(btn_position);
}

Position PlayerSet::getBbPosition(Position btn_position) const {
    return nextPosition(nextPosition(btn_position));
}

Position PlayerSet::getUtgPosition(Position btn_position) const {
    return nextPosition(getBbPosition(btn_position));
}
} // namespace holdem