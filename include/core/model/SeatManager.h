#pragma once

#include "GameTypes.hpp"
#include "PlayerSet.hpp"

namespace poker {
/**
 * @brief 玩家与座位管理类
 */
class SeatManager {
private:
    PlayerSet* player_set;
    seat_t btn_seat = 0;

public:
    SeatManager(PlayerSet* player_set) : player_set(player_set) {}
    ~SeatManager() = default;
    int playerCount() const { return player_set->playerCount(); }
    seat_t getBtnSeat() const { return btn_seat; }
    void setBtnSeat(seat_t new_btn_position) { btn_seat = new_btn_position; }
    void advanceBtnSeat() { btn_seat = nextSeat(btn_seat); }
    seat_t nextSeat(seat_t position) const { return (position + 1) % playerCount(); }
    seat_t nextActiveSeat(seat_t current) const;
    seat_t nextAliveSeat(seat_t current) const;
    seat_t getSbSeat() const { return (btn_seat + 1) % playerCount(); }
    seat_t getBbSeat() const { return (btn_seat + 2) % playerCount(); }
    seat_t getUtgSeat() const { return (btn_seat + 3) % playerCount(); }
    int distanceFromSeatLeft(seat_t seat, seat_t ref_seat);
    void sortSeats(std::vector<seat_t>& seats);
};
} // namespace poker