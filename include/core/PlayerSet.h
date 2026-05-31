#pragma once
#include <vector>

#include "Player.h"

namespace poker {
class PlayerSet {
private:
    std::vector<Player> players;
public:
    PlayerSet() = default;
    size_t size() const { return players.size(); }
    const std::vector<Player> & getPlayers() const { return players; }
    void addNewPlayer();
    Player& operator[] (seat_idx_t position) { return players[position]; }
    const Player& operator[] (seat_idx_t position) const { return players[position]; }
    Player& at(seat_idx_t position) { return players.at(position); }
    const Player& at(seat_idx_t position) const { return players.at(position); }
    auto begin()  { return players.begin(); }
    auto end()  { return players.end(); }
    auto cbegin() const { return players.cbegin(); }
    auto cend() const { return players.cend(); }
    seat_idx_t nextPosition(seat_idx_t position) const;
    seat_idx_t nextPositionToAct(seat_idx_t position) const;
    seat_idx_t getSbPosition(seat_idx_t btn_position) const;
    seat_idx_t getBbPosition(seat_idx_t btn_position) const;
    seat_idx_t getUtgPosition(seat_idx_t btn_position) const;
};
} // namespace holdem