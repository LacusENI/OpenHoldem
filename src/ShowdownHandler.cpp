#include "ShowdownHandler.h"

namespace holdem {
std::vector<HandValue> ShowdownHandler::evalHandValues(
        const std::vector<Player>& players,
        const Cards5& community_cards) {
    std::vector<HandValue> hand_values;
    for (const Player& player : players) {
        Cards7 hand7 = concatCards(player.hole_cards, community_cards);
        HandValue hand_value = HandEvaluator::evaluate(hand7);
        hand_values.push_back(hand_value);
    }
    return hand_values;
}

std::vector<Position> ShowdownHandler::determineWinners(
    const std::vector<HandValue>& hand_values) {
    HandValue winner_value;
    std::vector<Position> winners;
    for (Position pos = 0; pos < hand_values.size(); ++pos) {
        if (hand_values[pos] == winner_value) {
            winners.push_back(pos);
        } else if (hand_values[pos] > winner_value) {
            winner_value = hand_values[pos];
            winners.clear();
            winners.push_back(pos);
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