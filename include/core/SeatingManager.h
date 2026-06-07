#pragma once

#include <vector>

#include "Player.h"

namespace poker {
/**
 * @brief 玩家与座位管理类
 */
class SeatingManager {
private:
    std::vector<Player> players;
    seat_idx_t btn_position = 0;
public:
    SeatingManager() = default;
    size_t getPlayerCount() const {
        return players.size();
    }
    const std::vector<Player>& getPlayerList() const {
        return players;
    }
    std::vector<Player>& getPlayerList() {
        return players;
    }
    void addNewPlayer() {
        seat_idx_t position = players.size();
        players.emplace_back(position);
    }
    void setBtnPosition(seat_idx_t new_btn_position) {
        btn_position = new_btn_position;
    }
    seat_idx_t getBtnPosition() const {
        return btn_position;
    }
    void advanceBtnPosition() {
        btn_position = nextPosition(btn_position);
    }
    const Player& getPlayerAtSeat(seat_idx_t position) const {
        return players.at(position);
    }
    Player& getPlayerAtSeat(seat_idx_t position) {
        return players.at(position);
    }
    seat_idx_t nextPosition(seat_idx_t position) const {
        return (position + 1) % static_cast<int>(players.size());
    }
    seat_idx_t nextPositionToAct(seat_idx_t position) const {
        seat_idx_t next_pos = nextPosition(position);
        while (players[next_pos].is_folded && next_pos != position) {
            next_pos = nextPosition(next_pos);
        }
        return players[next_pos].is_folded ? -1 : next_pos;
    }
    seat_idx_t getSbPosition() const {
        return (btn_position + 1) % static_cast<int>(players.size());
    }
    seat_idx_t getBbPosition() const {
        return (btn_position + 2) % static_cast<int>(players.size());
    }
    seat_idx_t getUtgPosition() const {
        return (btn_position + 3) % static_cast<int>(players.size());
    }
    seat_idx_t getPostflopFirstPosition() const {
        return nextPositionToAct(btn_position);
    }
    seat_idx_t getPreflopFirstPosition() const {
        return getUtgPosition();
    }
};
} // namespace poker