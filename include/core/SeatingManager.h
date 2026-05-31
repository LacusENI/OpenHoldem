#pragma once

#include <vector>

#include "Player.h"

namespace poker {
class SeatingManager {
private:
    std::vector<Player> players;
    Position btn_position;
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
        Position position = players.size();
        players.emplace_back(position);
    }
    void setBtnPosition(Position new_btn_position) {
        btn_position = new_btn_position;
    }
    Position getBtnPosition() const {
        return btn_position;
    }
    void advanceBtnPosition() {
        btn_position = nextPosition(btn_position);
    }
    const Player& getPlayerAt(Position position) const {
        return players.at(position);
    }
    Player& getPlayerAt(Position position) {
        return players.at(position);
    }
    Position nextPosition(Position position) const {
        return (position + 1) % static_cast<int>(players.size());
    }
    Position nextPositionToAct(Position position) const {
        Position next_pos = nextPosition(position);
        while (players[next_pos].is_folded && next_pos != position) {
            next_pos = nextPosition(next_pos);
        }
        return players[next_pos].is_folded ? -1 : next_pos;
    }
    Position getSbPosition() const {
        return (btn_position + 1) % static_cast<int>(players.size());
    }
    Position getBbPosition() const {
        return (btn_position + 2) % static_cast<int>(players.size());
    }
    Position getUtgPosition() const {
        return (btn_position + 3) % static_cast<int>(players.size());
    }
    Position getPostflopFirstPosition() const {
        return nextPositionToAct(btn_position);
    }
    Position getPreflopFirstPosition() const {
        return getUtgPosition();
    }
};

} // namespace poker