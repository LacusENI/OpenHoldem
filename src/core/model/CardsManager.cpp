#include "CardsManager.h"

#include <algorithm>
#include <vector>

#include "Card.hpp"
#include "Deck.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"
#include "Player.hpp"
#include "PlayerSet.h"
#include "eval_hand.h"

namespace poker {
cards_2_t CardsManager::getHoleCardsAt(seat_idx_t seat_idx) const {
    return player_set->playerAt(seat_idx).hole_cards;
}

void CardsManager::setHoleCardsAt(seat_idx_t seat_idx, cards_2_t hole_cards) {
    player_set->playerAt(seat_idx).hole_cards = hole_cards;
}

void CardsManager::dealCommunityCards(Stage stage) {
    switch (stage) {
    case Stage::PREFLOP: break;
    case Stage::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case Stage::TURN: community_cards[3] = deck->deal(); break;
    case Stage::RIVER: community_cards[4] = deck->deal(); break;
    default: break;
    }
}

void CardsManager::dealHoleCards() {
    for (Player& player : *player_set) {
        player.hole_cards = {deck->deal(), deck->deal()};
    }
}

HandValue CardsManager::computePlayerHandValue(seat_idx_t seat_idx) {
    cards_7_t seven_cards;
    cards_2_t hole_cards = getHoleCardsAt(seat_idx);
    std::ranges::copy(community_cards, seven_cards.begin());
    std::ranges::copy(hole_cards, seven_cards.begin() + community_cards.size());
    return algo::eval_hand(seven_cards);
}

std::vector<seat_idx_t> CardsManager::determineWinners() {
    std::vector<seat_idx_t> winners;
    HandValue max_hand_value(0);
    for (seat_idx_t active_seat : player_set->getActiveSeats()) {
        HandValue hand_value = computePlayerHandValue(active_seat);
        if (hand_value > max_hand_value) {
            winners.clear();
            max_hand_value = hand_value;
            winners.push_back(active_seat);
        } else if (hand_value == max_hand_value) {
            winners.push_back(active_seat);
        }
    }
    return winners;
}
} // namespace poker