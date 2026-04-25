#pragma once
#include <vector>

#include "Player.h"

namespace holdem {
class PlayerSet {
private:
    std::vector<Player> players;
public:
    PlayerSet() = default;
    size_t size() const { return players.size(); }
    const std::vector<Player> & getPlayers() const { return players; }
    void addNewPlayer();
    Player& operator[] (Position position) { return players[position]; }
    const Player& operator[] (Position position) const { return players[position]; }
    Player& at(Position position) { return players.at(position); }
    const Player& at(Position position) const { return players.at(position); }
    auto begin()  { return players.begin(); }
    auto end()  { return players.end(); }
    auto cbegin() const { return players.cbegin(); }
    auto cend() const { return players.cend(); }
    Position nextPosition(Position position) const;
    Position nextPositionToAct(Position position) const;
};
} // namespace holdem