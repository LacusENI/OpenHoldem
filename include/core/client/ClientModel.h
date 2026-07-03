#pragma once

#include <vector>

#include "Card.h"
#include "GameTypes.h"
#include "Player.h"

namespace poker {
class ClientModel {
public:
    cards_5_t community_cards;
    std::vector<Player> players;
    std::vector<PlayerState> player_states;
    std::vector<chips_t> player_bets;
    seat_idx_t this_player_seat;
    seat_idx_t btn_seat{};
    chips_t big_blind{};
    chips_t pot{};
};
} // namespace poker