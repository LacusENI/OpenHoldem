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
    int size;
    switch (stage) {
    case Stage::FLOP: size = 3; break;
    case Stage::TURN: size = 4; break;
    case Stage::RIVER: size = 5; break;
    case Stage::SHOWDOWN: size = 5; break;
    default: return;
    }
    for (int i = 0; i < size; i++) {
        if (community_cards[i] == Card()) {
            community_cards[i] = deck->deal();
        }
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