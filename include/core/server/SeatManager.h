#pragma once

#include <vector>

#include "Player.h"

namespace poker {
/**
 * @brief 玩家与座位管理类
 */
class SeatManager {
private:
    std::vector<Player> players;
    seat_idx_t btn_seat = 0;

public:
    SeatManager() = default;
    size_t getPlayerCount() const { return players.size(); }
    const std::vector<Player>& getPlayerList() const { return players; }
    std::vector<Player>& getPlayerList() { return players; }
    void addNewPlayer() {
        seat_idx_t position = players.size();
        players.emplace_back(position);
    }
    void setBtnSeat(seat_idx_t new_btn_position) { btn_seat = new_btn_position; }
    seat_idx_t getBtnSeat() const { return btn_seat; }
    void advanceBtnSeat() { btn_seat = nextSeat(btn_seat); }
    const Player& getPlayerAtSeat(seat_idx_t position) const { return players.at(position); }
    Player& getPlayerAtSeat(seat_idx_t position) { return players.at(position); }
    seat_idx_t nextSeat(seat_idx_t position) const {
        return (position + 1) % static_cast<int>(players.size());
    }
    seat_idx_t findNextSeatToAct(seat_idx_t position) const {
        seat_idx_t next_pos = nextSeat(position);
        while (players[next_pos].is_folded && next_pos != position) {
            next_pos = nextSeat(next_pos);
        }
        return players[next_pos].is_folded ? -1 : next_pos;
    }
    seat_idx_t getSbSeat() const { return (btn_seat + 1) % static_cast<int>(players.size()); }
    seat_idx_t getBbSeat() const { return (btn_seat + 2) % static_cast<int>(players.size()); }
    seat_idx_t getUtgSeat() const { return (btn_seat + 3) % static_cast<int>(players.size()); }
    seat_idx_t getPostflopFirstActorSeat() const { return findNextSeatToAct(btn_seat); }
    seat_idx_t getPreflopFirstActorSeat() const { return getUtgSeat(); }
};
} // namespace poker