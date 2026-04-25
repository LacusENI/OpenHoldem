#include <algorithm>
#include <chrono>

#include "Deck.h"

constexpr int CARDS_SIZE = 52;

namespace holdem {
Deck::Deck() {
    cards.reserve(CARDS_SIZE);
    for (int s = MIN_SUIT; s <= MAX_SUIT; ++s) {
        for (int r = MIN_RANK; r <= MAX_RANK; ++r) {
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
    if (next_card_index >= CARDS_SIZE) {
        throw std::runtime_error("Deck::deal: index out of range");
    }
    return cards[next_card_index++];
}
}
