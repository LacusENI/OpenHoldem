#include "PlayerSet.h"

#include <algorithm>
#include <ranges>

namespace poker {
auto PlayerSet::getActivePlayers() {
    return players | std::ranges::views::filter([](const Player& p) { return p.isActive(); });
}
auto PlayerSet::getActiveSeats() {
    return std::views::iota(0, playerCount()) |
           std::views::filter([&](seat_idx_t seat) { return this->playerAt(seat).isActive(); });
}
int PlayerSet::getActivePlayerCount() {
    return std::ranges::count_if(players, [](const Player& p) { return p.isActive(); });
}
auto PlayerSet::getActionablePlayers() {
    return players | std::ranges::views::filter([](const Player& p) { return p.canAct(); });
}
int PlayerSet::getActionablePlayerCount() {
    return std::ranges::count_if(players, [](const Player& p) { return p.canAct(); });
}
} // namespace poker