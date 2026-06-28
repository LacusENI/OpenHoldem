#include "ShowdownHandler.h"

#include <algorithm>

#include "eval_hand.h"

namespace poker {
void join_community_cards(cards_7_t& cards7, const cards_5_t& cards5) {
    std::ranges::copy(cards5, cards7.begin());
}

void join_hole_cards(cards_7_t& cards7, const cards_2_t& cards2) {
    std::ranges::copy(cards2, cards7.begin() + 2);
}

std::vector<std::pair<seat_idx_t, HandValue>> ShowdownHandler::evalHandValues(
    const std::vector<Player>& players, const cards_5_t& community_cards) {
    std::vector<std::pair<seat_idx_t, HandValue>> result;
    cards_7_t seven_cards;
    join_community_cards(seven_cards, community_cards);
    for (const Player& player : players) {
        if (player.isActive()) {
            join_hole_cards(seven_cards, player.hole_cards);
            HandValue hand_value = algo::eval_hand(seven_cards);
            result.emplace_back(player.position, hand_value);
        }
    }
    return result;
}

std::vector<seat_idx_t> ShowdownHandler::determineWinners(
    const std::vector<std::pair<seat_idx_t, HandValue>>& hand_values) {
    HandValue winner_value;
    std::vector<seat_idx_t> winners;
    for (auto [position, hand_value] : hand_values) {
        if (hand_value > winner_value) {
            winner_value = hand_value;
            winners = {position};
        } else if (hand_value == winner_value) {
            winners.push_back(position);
        }
    }
    return winners;
}

std::vector<chips_t> ShowdownHandler::calculateDistribution(const int pot, const int winners_n) {
    const int share = pot / winners_n;
    const int remainder = pot % winners_n;

    std::vector<chips_t> amounts;
    for (size_t i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        amounts.push_back(amount);
    }
    return amounts;
}

} // namespace poker