#include "SeatManager.h"

#include "GameTypes.hpp"

namespace poker {
seat_idx_t SeatManager::nextActiveSeat(seat_idx_t current) const {
    seat_idx_t next = nextSeat(current);
    while (!player_set->playerAt(next).isActive() && next != current) {
        next = nextSeat(next);
    }
    return player_set->playerAt(next).isActive() ? next : -1;
}
seat_idx_t SeatManager::nextAliveSeat(seat_idx_t current) const {
    seat_idx_t next = nextSeat(current);
    while (!player_set->playerAt(next).isAlive() && next != current) {
        next = nextSeat(next);
    }
    return player_set->playerAt(next).isAlive() ? next : -1;
}
int SeatManager::distanceFromSeatLeft(seat_idx_t seat, seat_idx_t ref_seat) {
    return (seat - ref_seat + playerCount()) % playerCount();
}
void SeatManager::sortSeats(std::vector<seat_idx_t>& seats) {
    seat_idx_t ref_seat = nextSeat(getBtnSeat());
    std::ranges::sort(seats, [&ref_seat, this](seat_idx_t seat_a, seat_idx_t seat_b) {
        return this->distanceFromSeatLeft(seat_a, ref_seat) <
               this->distanceFromSeatLeft(seat_b, ref_seat);
    });
}
} // namespace poker