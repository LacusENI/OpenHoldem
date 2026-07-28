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
    auto getActivePlayers() {
        return players | std::ranges::views::filter([](const Player& p) { return p.isActive(); });
    }
    auto getActiveSeats() {
        return std::views::iota(0, playerCount()) |
               std::views::filter([&](seat_idx_t seat) { return this->playerAt(seat).isActive(); });
    }
    int getActivePlayerCount() {
        return std::ranges::count_if(players, [](const Player& p) { return p.isActive(); });
    }
    auto getActionablePlayers() {
        return players | std::ranges::views::filter([](const Player& p) { return p.canAct(); });
    }
    int getActionablePlayerCount() {
        return std::ranges::count_if(players, [](const Player& p) { return p.canAct(); });
    }
};
} // namespace poker