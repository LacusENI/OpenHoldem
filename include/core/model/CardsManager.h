#include <vector>

#include "Card.hpp"
#include "GameTypes.hpp"

namespace poker {
class IDeck;

class CardsManager {
private:
    IDeck* deck;
    cards_5_t community_cards;
    std::vector<cards_2_t> hole_cards;

public:
    CardsManager(IDeck* deck) : deck(deck) {}
    ~CardsManager() = default;

    void set_player_count(int player_count) { hole_cards.resize(player_count); }
    cards_5_t get_community_cards() const { return community_cards; }
    cards_2_t get_hole_cards_at(seat_idx_t seat_idx) const { return hole_cards[seat_idx]; }
    void dealCommunityCards(Stage stage);
    void dealHoleCards();
};
} // namespace poker