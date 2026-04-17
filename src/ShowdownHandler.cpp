#include "ShowdownHandler.h"

namespace holdem {
std::vector<std::pair<Position, HandValue>> ShowdownHandler::evalHandValues(
    const std::vector<Player>& players,
    const Cards5& community_cards) {
    std::vector<std::pair<Position, HandValue>> result;
    for (const Player& player : players) {
        if (!player.is_folded) {
            Cards7 hand7 = concatCards(player.hole_cards, community_cards);
            HandValue hand_value = HandEvaluator::evaluate(hand7);
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

std::vector<Stack> ShowdownHandler::calculateDistribution(int pot, int winners_n) {
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