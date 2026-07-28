#pragma once

#include "Card.hpp"
#include "GameTypes.hpp"

namespace poker {
class IDeck;
class PlayerSet;

class CardsManager {
private:
    IDeck* deck;
    PlayerSet* player_set;
    cards_5_t community_cards;

public:
    CardsManager(IDeck* deck) : deck(deck) {}
    ~CardsManager() = default;

    cards_5_t getCommunityCards() const { return community_cards; }
    cards_2_t getHoleCardsAt(seat_idx_t seat_idx) const;
    void dealCommunityCards(Stage stage);
    void dealHoleCards();
};
} // namespace poker