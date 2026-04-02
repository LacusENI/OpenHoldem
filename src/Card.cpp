#include "Card.h"
#include <vector>

namespace holdem {
std::string Card::toString() const {
    static std::vector<std::string> suits = {
        "C", "D", "H", "S" // Clubs, Diamonds, Hearts, Spades
    };
    static std::vector<std::string> ranks = {
        "", "", "2", "3", "4", "5", "6", "7",
        "8", "9", "T", "J", "Q", "K", "A"
    };
    auto s_idx = static_cast<int>(suit);
    auto r_idx = static_cast<int>(rank);
    return std::string(suits[s_idx] + ranks[r_idx]);
}

Cards7 concatCards(const Cards2& cards2, const Cards5& cards5) {
    Cards7 cards7;
    std::copy(cards2.begin(), cards2.end(), cards7.begin());
    std::copy(cards5.begin(), cards5.end(), cards7.begin() + 2);
    return cards7;
}

}