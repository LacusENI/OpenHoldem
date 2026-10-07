#pragma once

#include <vector>

#include "GameTypes.hpp"
#include "side_pots.h"

namespace poker {
class PlayerSet;
class SeatManager;

class ChipsManager {
private:
    PlayerSet& player_set;
    SeatManager& seat_manager;
    std::vector<Pot> pots;

public:
    ChipsManager(PlayerSet& player_set, SeatManager& seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~ChipsManager() = default;

    const std::vector<Pot>& getPots() const { return pots; }
    /* 清空所有底池（用于新的一手牌） */
    void clearPots() { pots.clear(); }
    chips_t getPlayerChipsAt(seat_t seat_idx);
    void setPlayerChipsAt(seat_t seat_idx, chips_t player_chips);

    void splitThePot();
    /* 无人竞争时，将全部底池直接判给唯一存活玩家（不切分边池） */
    void awardPotTo(seat_t winner);
    /* 清零所有玩家的本局投入（结算后调用） */
    void clearCommitted();

    void moveChipsToPot(seat_t player_seat, chips_t chips);
    void moveChipsToPlayer(seat_t player_seat, chips_t chips);
    void distributePot(chips_t pot_chips, std::vector<seat_t>& winners);
};
} // namespace poker