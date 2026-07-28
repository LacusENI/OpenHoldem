#pragma once

#include "Player.hpp"
#include "PlayerSet.h"

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
    seat_idx_t findNextActorSeat(seat_idx_t position) const {
        seat_idx_t next_pos = nextSeat(position);
        while (player_set->playerAt(next_pos).is_folded && next_pos != position) {
            next_pos = nextSeat(next_pos);
        }
        return player_set->playerAt(next_pos).is_folded ? -1 : next_pos;
    }
    seat_idx_t getSbSeat() const { return (btn_seat + 1) % playerCount(); }
    seat_idx_t getBbSeat() const { return (btn_seat + 2) % playerCount(); }
    seat_idx_t getUtgSeat() const { return (btn_seat + 3) % playerCount(); }
    seat_idx_t getPostflopFirstActorSeat() const { return findNextActorSeat(btn_seat); }
    seat_idx_t getPreflopFirstActorSeat() const { return getUtgSeat(); }
};
} // namespace poker