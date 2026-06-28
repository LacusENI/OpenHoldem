#include "Card.h"

#include <format>
#include <stdexcept>
#include <string>
#include <vector>

namespace poker {
Card::Card(const char* str) {
    switch (str[0]) {
    case '_':
        suit = Suit::NONE;
        break;
    case 'C':
        suit = Suit::CLUB;
        break;
    case 'D':
        suit = Suit::DIAMOND;
        break;
    case 'H':
        suit = Suit::HEART;
        break;
    case 'S':
        suit = Suit::SPADE;
        break;
    default:
        throw std::invalid_argument("invalid card string: " + std::string(str));
    }
    switch (str[1]) {
    case '_':
        rank = Rank::NONE;
        break;
    case 'T':
        rank = Rank::TEN;
        break;
    case 'J':
        rank = Rank::JACK;
        break;
    case 'Q':
        rank = Rank::QUEEN;
        break;
    case 'K':
        rank = Rank::KING;
        break;
    case 'A':
        rank = Rank::ACE;
        break;
    default:
        if (str[1] >= '2' && str[1] <= '9') {
            rank = static_cast<Rank>(str[1] - '0');
        } else {
            throw std::invalid_argument("invalid card string: " + std::string(str));
        }
    }
}

std::string to_string(Suit suit) {
    static std::vector<std::string> suits = {"_", "C", "D", "H", "S"};
    return suits[static_cast<int>(suit)];
}

std::string to_string(Rank rank) {
    static std::vector<std::string> ranks = {"_", "?", "2", "3", "4", "5", "6", "7",
                                             "8", "9", "T", "J", "Q", "K", "A"};
    return ranks[static_cast<int>(rank)];
}

std::string to_string(Card card) {
    return to_string(card.suit) + to_string(card.rank);
}

std::string to_string(const cards_2_t& cards) {
    return std::format("[{}, {}]", to_string(cards[0]), to_string(cards[1]));
}

std::string to_string(const cards_5_t& cards) {
    return std::format("[{}, {}, {}, {}, {}]", to_string(cards[0]), to_string(cards[1]),
                       to_string(cards[2]), to_string(cards[3]), to_string(cards[4]));
}

std::string to_string(const cards_7_t& cards) {
    return std::format("[{}, {}, {}, {}, {}, {}, {}]", to_string(cards[0]), to_string(cards[1]),
                       to_string(cards[2]), to_string(cards[3]), to_string(cards[4]),
                       to_string(cards[5]), to_string(cards[6]));
}

std::ostream& operator<<(std::ostream& os, Suit suit) {
    return os << to_string(suit);
}

std::ostream& operator<<(std::ostream& os, Rank rank) {
    return os << to_string(rank);
}

std::ostream& operator<<(std::ostream& os, Card card) {
    return os << to_string(card);
}

std::ostream& operator<<(std::ostream& os, const cards_2_t& cards) {
    return os << to_string(cards);
}

std::ostream& operator<<(std::ostream& os, const cards_5_t& cards) {
    return os << to_string(cards);
}

std::ostream& operator<<(std::ostream& os, const cards_7_t& cards) {
    return os << to_string(cards);
}
} // namespace poker