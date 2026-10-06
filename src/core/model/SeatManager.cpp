#include "SeatManager.h"

#include "GameTypes.hpp"

namespace poker {
seat_t SeatManager::nextActiveSeat(seat_t current) const {
    seat_t next = nextSeat(current);
    while (!player_set->playerAt(next).isActive() && next != current) {
        next = nextSeat(next);
    }
    return player_set->playerAt(next).isActive() ? next : INVALID_SEAT;
}
seat_t SeatManager::nextAliveSeat(seat_t current) const {
    seat_t next = nextSeat(current);
    while (!player_set->playerAt(next).isAlive() && next != current) {
        next = nextSeat(next);
    }
    return player_set->playerAt(next).isAlive() ? next : INVALID_SEAT;
}
seat_t SeatManager::nextSeatNeedsToAct(seat_t current, chips_t round_bet) const {
    if (playerCount() == 0) return INVALID_SEAT;
    // current 非法时从最后一个座位开始，保证扫描覆盖全部座位
    const seat_t start = (current == INVALID_SEAT) ? playerCount() - 1 : current;
    seat_t next = nextSeat(start);
    while (next != start && !player_set->playerAt(next).needsToAct(round_bet)) {
        next = nextSeat(next);
    }
    return player_set->playerAt(next).needsToAct(round_bet) ? next : INVALID_SEAT;
}
int SeatManager::distanceFromSeatLeft(seat_t seat, seat_t ref_seat) {
    return (seat - ref_seat + playerCount()) % playerCount();
}
void SeatManager::sortSeats(std::vector<seat_t>& seats) {
    seat_t ref_seat = nextSeat(getBtnSeat());
    std::ranges::sort(seats, [&ref_seat, this](seat_t seat_a, seat_t seat_b) {
        return this->distanceFromSeatLeft(seat_a, ref_seat) <
               this->distanceFromSeatLeft(seat_b, ref_seat);
    });
}
} // namespace poker