#include <string>
#include <stdexcept>
#include <vector>
#include <format>

#include "entity/Card.h"

namespace holdem {
namespace utils {
std::string castSuitToString(Suit suit) {
    static std::vector<std::string> suits = {
        "_", "C", "D", "H", "S"
    };
    return suits[static_cast<int>(suit)];
}
std::string castRankToString(Rank rank) {
    static std::vector<std::string> ranks = {
        "_", "?", "2", "3", "4", "5", "6", "7",
        "8", "9", "T", "J", "Q", "K", "A"
    };
    return ranks[static_cast<int>(rank)];
}
}

std::ostream& operator<<(std::ostream& os, const Card& card){
    return os << std::format("Card(\"{}\")", card.toString());
}

Card::Card(const char* str) {
    switch (str[0]) {
    case '_': suit = Suit::NONE; break;
    case 'C': suit = Suit::CLUB; break;
    case 'D': suit = Suit::DIAMOND; break;
    case 'H': suit = Suit::HEART; break;
    case 'S': suit = Suit::SPADE; break;
    default: throw std::invalid_argument("invalid card string: " + std::string(str));
    }
    switch (str[1]) {
    case '_': rank = Rank::NONE; break;
    case 'T': rank = Rank::TEN; break;
    case 'J': rank = Rank::JACK; break;
    case 'Q': rank = Rank::QUEEN; break;
    case 'K': rank = Rank::KING; break;
    case 'A': rank = Rank::ACE; break;
    default:
        if (str[1] >= '2' && str[1] <= '9') {
            rank = static_cast<Rank>(str[1] - '0');
        } else {
            throw std::invalid_argument("invalid card string: " + std::string(str));
        }
    }
}

std::string Card::toMessage() const {
    static std::vector<std::string> suits = {
        "_", "\u2663", "\u2666", "\u2665", "\u2660"
    }; // None, Club, Diamond, Heart, Spade
    return suits[static_cast<int>(suit)] + utils::castRankToString(rank);
}

std::string Card::toString() const {
    return utils::castSuitToString(suit) + utils::castRankToString(rank);
}

Cards7 concatCards(const Cards2& cards2, const Cards5& cards5) {
    Cards7 cards7;
    std::ranges::copy(cards2, cards7.begin());
    std::ranges::copy(cards5, cards7.begin() + 2);
    return cards7;
}

}