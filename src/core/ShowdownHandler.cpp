#include <algorithm>

#include "ShowdownHandler.h"
#include "eval_hand.h"

namespace poker {
Cards7 concatCards(const Cards2& cards2, const Cards5& cards5) {
    Cards7 cards7;
    std::ranges::copy(cards2, cards7.begin());
    std::ranges::copy(cards5, cards7.begin() + 2);
    return cards7;
}

std::vector<std::pair<Position, HandValue>> ShowdownHandler::evalHandValues(
        const std::vector<Player>& players,
        const Cards5& community_cards) {
    std::vector<std::pair<Position, HandValue>> result;
    Cards7 hand = concatCards({}, community_cards);
    for (const Player& player : players) {
        if (player.isActive()) {
            hand[0] = player.hole_cards[0];
            hand[1] = player.hole_cards[1];
            HandValue hand_value = math::eval_hand(hand);
            result.emplace_back(player.position, hand_value);
        }
    }
    return result;
}

std::vector<Position> ShowdownHandler::determineWinners(
    const std::vector<std::pair<Position, HandValue>>& hand_values) {
    HandValue winner_value;
    std::vector<Position> winners;
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

std::vector<Stack> ShowdownHandler::calculateDistribution(
    const int pot, const int winners_n) {
    const int share = pot / winners_n;
    const int remainder = pot % winners_n;

    std::vector<Stack> amounts;
    for (size_t i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        amounts.push_back(amount);
    }
    return amounts;
}

}