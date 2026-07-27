#include "CardsManager.h"

#include "Deck.hpp"

namespace poker {
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
    for (auto& cards : hole_cards) {
        cards = {deck->deal(), deck->deal()};
    }
}
} // namespace poker