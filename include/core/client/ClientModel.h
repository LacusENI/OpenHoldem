#pragma once

#include <vector>

#include "Card.h"
#include "GameTypes.h"
#include "Player.h"

namespace poker {
class ClientModel {
public:
    chips_t pot{};
    std::vector<Player> players;
    seat_idx_t btn_seat{};
    Stage current_stage{};
    chips_t big_blind{};
    cards_5_t community_cards;
    std::vector<chips_t> player_bets;
    chips_t round_bet = 0;
    chips_t min_raise_delta = 0;
};
} // namespace poker