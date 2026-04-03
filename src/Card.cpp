#include "Card.h"

#include <stdexcept>
#include <vector>

namespace holdem {

std::string rankToString(Rank rank) {
    static std::vector<std::string> ranks = {
        "", "", "2", "3", "4", "5", "6", "7",
        "8", "9", "T", "J", "Q", "K", "A"
    };
    return ranks[static_cast<int>(rank)];
}

Card::Card(const std::string& str) {
    switch (str[0]) {
    case 'C': suit = Suit::CLUB; break;
    case 'D': suit = Suit::DIAMOND; break;
    case 'H': suit = Suit::HEART; break;
    case 'S': suit = Suit::SPADE; break;
    default: throw std::invalid_argument("invalid card string: " + str);
    }
    switch (str[1]) {
    case 'T': rank = Rank::TEN; break;
    case 'J': rank = Rank::JACK; break;
    case 'Q': rank = Rank::QUEEN; break;
    case 'K': rank = Rank::KING; break;
    case 'A': rank = Rank::ACE; break;
    default:
        if (str[1] >= '2' && str[1] <= '9') {
            rank = static_cast<Rank>(str[1] - '0');
        } else {
            throw std::invalid_argument("invalid card string: " + str);
        }
    }
}

std::string Card::toMessage() const {
    static std::vector<std::string> suits = {
        "\u2663", "\u2666", "\u2665", "\u2660"
    }; // Club, Diamond, Heart, Spade
    return suits[static_cast<int>(suit)] + rankToString(rank);
}

std::string Card::toString() const {
    static std::vector<std::string> suits = {
        "C", "D", "H", "S"
    }; // Club, Diamond, Heart, Spade
    return suits[static_cast<int>(suit)] + rankToString(rank);
}

Cards7 concatCards(const Cards2& cards2, const Cards5& cards5) {
    Cards7 cards7;
    std::copy(cards2.begin(), cards2.end(), cards7.begin());
    std::copy(cards5.begin(), cards5.end(), cards7.begin() + 2);
    return cards7;
}

}