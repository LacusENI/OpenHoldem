#include "CardsManager.h"

#include "Deck.hpp"
#include "Player.hpp"
#include "PlayerSet.h"

namespace poker {
cards_2_t CardsManager::getHoleCardsAt(seat_idx_t seat_idx) const {
    return player_set->playerAt(seat_idx).hole_cards;
}

void CardsManager::dealCommunityCards(Stage stage) {
    switch (stage) {
    case Stage::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case Stage::TURN: community_cards[3] = deck->deal(); break;
    case Stage::RIVER: community_cards[4] = deck->deal(); break;
    default: throw std::invalid_argument("Invalid stage for dealing community cards");
    }
}

void CardsManager::dealHoleCards() {
    for (Player& player : *player_set) {
        player.hole_cards = {deck->deal(), deck->deal()};
    }
}
} // namespace poker