#pragma once

#include <vector>

#include "Card.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"

namespace poker {
class IDeck;
class PlayerSet;

class CardsManager {
private:
    IDeck* deck;
    PlayerSet* player_set;
    cards_5_t community_cards;

public:
    CardsManager(IDeck* deck, PlayerSet* player_set) : deck(deck), player_set(player_set) {}
    ~CardsManager() = default;

    cards_5_t getCommunityCards() const { return community_cards; }
    void setCommunityCards(cards_5_t cc) { community_cards = cc; }
    /* 清空公共牌（用于新的一手牌） */
    void clearCommunityCards() { community_cards = {}; }
    cards_2_t getHoleCardsAt(seat_t seat_idx) const;
    void setHoleCardsAt(seat_t seat_idx, cards_2_t hole_cards);

    void shuffleDeck();
    void dealHoleCards();
    void dealCommunityCards(Stage stage);
    HandValue evaluatePlayerHand(seat_t seat_idx);
    std::vector<seat_t> determineWinners(const std::vector<seat_t>& contenders);
};
} // namespace poker