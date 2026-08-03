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
    cards_2_t getHoleCardsAt(seat_idx_t seat_idx) const;
    void setHoleCardsAt(seat_idx_t seat_idx, cards_2_t hole_cards);

    void dealCommunityCards(Stage stage);
    void dealHoleCards();
    HandValue evaluatePlayerHand(seat_idx_t seat_idx);
    ShowdownResult processShowdown();
    std::vector<seat_idx_t> determineWinners(const ShowdownResult& showdown_result);
};
} // namespace poker