#include "CardsManager.h"

#include <algorithm>
#include <vector>

#include "Card.hpp"
#include "Deck.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"
#include "Player.hpp"
#include "PlayerSet.hpp"
#include "eval_hand.h"

namespace poker {
cards_2_t CardsManager::getHoleCardsAt(seat_t seat_idx) const {
    return player_set->playerAt(seat_idx).getHoleCards();
}

void CardsManager::setHoleCardsAt(seat_t seat_idx, cards_2_t hole_cards) {
    player_set->playerAt(seat_idx).setHoleCards(hole_cards);
}

void CardsManager::shuffleDeck() {
    deck->shuffle();
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
        player.setHoleCards({deck->deal(), deck->deal()});
    }
}

HandValue CardsManager::evaluatePlayerHand(seat_t seat_idx) {
    cards_7_t seven_cards;
    cards_2_t hole_cards = getHoleCardsAt(seat_idx);
    std::ranges::copy(community_cards, seven_cards.begin());
    std::ranges::copy(hole_cards, seven_cards.begin() + community_cards.size());
    return algo::eval_hand(seven_cards);
}

std::vector<seat_t> CardsManager::determineWinners(const std::vector<seat_t>& players) {
    std::vector<seat_t> winners;
    HandValue max_hand_value;
    for (seat_t player_seat : players) {
        HandValue hand_value = evaluatePlayerHand(player_seat);
        if (hand_value > max_hand_value) {
            winners.clear();
            max_hand_value = hand_value;
            winners.push_back(player_seat);
        } else if (hand_value == max_hand_value) {
            winners.push_back(player_seat);
        }
    }
    return winners;
}
} // namespace poker