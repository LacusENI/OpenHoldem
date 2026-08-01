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
    seat_idx_t btn_seat = 0;

public:
    SeatManager(PlayerSet* player_set) : player_set(player_set) {}
    ~SeatManager() = default;
    int playerCount() const { return player_set->playerCount(); }
    seat_idx_t getBtnSeat() const { return btn_seat; }
    void setBtnSeat(seat_idx_t new_btn_position) { btn_seat = new_btn_position; }
    void advanceBtnSeat() { btn_seat = nextSeat(btn_seat); }
    seat_idx_t nextSeat(seat_idx_t position) const { return (position + 1) % playerCount(); }
    seat_idx_t nextActiveSeat(seat_idx_t current) const;
    seat_idx_t nextAliveSeat(seat_idx_t current) const;
    seat_idx_t getSbSeat() const { return (btn_seat + 1) % playerCount(); }
    seat_idx_t getBbSeat() const { return (btn_seat + 2) % playerCount(); }
    seat_idx_t getUtgSeat() const { return (btn_seat + 3) % playerCount(); }
    int distanceFromSeatLeft(seat_idx_t seat, seat_idx_t ref_seat);
    void sortSeats(std::vector<seat_idx_t>& seats);
};
} // namespace poker