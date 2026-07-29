#include "SeatManager.h"

#include "GameTypes.hpp"

namespace poker {
seat_idx_t SeatManager::nextActiveSeat(seat_idx_t position) const {
    seat_idx_t next_pos = nextSeat(position);
    while (player_set->playerAt(next_pos).is_folded && next_pos != position) {
        next_pos = nextSeat(next_pos);
    }
    return player_set->playerAt(next_pos).is_folded ? -1 : next_pos;
}
seat_idx_t SeatManager::nextActorSeat(seat_idx_t position) const {
    seat_idx_t next_pos = nextSeat(position);
    while (player_set->playerAt(next_pos).is_folded && next_pos != position) {
        next_pos = nextSeat(next_pos);
    }
    return player_set->playerAt(next_pos).is_folded ? -1 : next_pos;
}
seat_idx_t SeatManager::getSbSeat() const {
    return (btn_seat + 1) % playerCount();
}
seat_idx_t SeatManager::getBbSeat() const {
    return (btn_seat + 2) % playerCount();
}
seat_idx_t SeatManager::getUtgSeat() const {
    return (btn_seat + 3) % playerCount();
}
seat_idx_t SeatManager::getPostflopFirstActorSeat() const {
    return nextActorSeat(btn_seat);
}
seat_idx_t SeatManager::getPreflopFirstActorSeat() const {
    return getUtgSeat();
}
int SeatManager::distanceFromSeatLeft(seat_idx_t seat, seat_idx_t ref_seat) {
    return (seat - ref_seat + playerCount()) % playerCount();
}
} // namespace poker