#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

#include "GameTypes.hpp"
#include "Player.hpp"

namespace poker {
class PlayerSet {
private:
    std::vector<Player> players;

public:
    PlayerSet() = default;
    ~PlayerSet() = default;

    const Player& playerAt(seat_idx_t seat_idx) const { return players[seat_idx]; }
    Player& playerAt(seat_idx_t seat_idx) { return players[seat_idx]; }
    auto begin() { return players.begin(); }
    auto end() { return players.end(); }
    auto begin() const { return players.begin(); }
    auto end() const { return players.end(); }
    int playerCount() { return players.size(); }
    void resize(int new_size) { players.resize(new_size); }
    auto getAlivePlayers() {
        return std::views::iota(0, playerCount()) | std::views::filter([this](seat_idx_t seat) {
                   return this->playerAt(seat).isAlive();
               });
    }
    int getAlivePlayerCount() {
        return std::ranges::count_if(players, [](const Player& p) { return p.isAlive(); });
    }
    int getActivePlayerCount() {
        return std::ranges::count_if(players, [](const Player& p) { return p.isActive(); });
    }
};
} // namespace poker