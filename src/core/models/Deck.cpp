#include <algorithm>
#include <chrono>

#include "models/Deck.h"

constexpr int cards_size = 52;

namespace holdem {
Deck::Deck() {
    cards.reserve(cards_size);
    for (int s = 0; s < 4; ++s) {
        for (int r = 2; r <= 14; ++r) {
            cards.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
        }
    }
    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
}

void Deck::shuffle() {
    std::ranges::shuffle(cards, rng);
    next_card_index = 0;
}

Card Deck::deal() {
    if (next_card_index >= cards.size()) {
        throw std::runtime_error("Deck::deal: index out of range");
    }
    return cards[next_card_index++];
}
}
