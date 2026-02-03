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
}