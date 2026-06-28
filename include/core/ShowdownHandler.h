#pragma once

#include <vector>

#include "GameTypes.h"
#include "HandValue.h"
#include "Player.h"

namespace poker {

class ShowdownHandler {
public:
    static std::vector<std::pair<seat_idx_t, HandValue>> evalHandValues(
        const std::vector<Player>& players, const cards_5_t& community_cards);

    static std::vector<seat_idx_t> determineWinners(
        const std::vector<std::pair<seat_idx_t, HandValue>>& hand_values);

    static std::vector<chips_t> calculateDistribution(int pot, int winners_n);
};

} // namespace poker