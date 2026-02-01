#ifndef OPENHOLDEM_CARD_H
#define OPENHOLDEM_CARD_H
#include <cstdint>
#include <string>
namespace holdem {

enum class Suit : uint8_t {
    CLUBS, DIAMONDS, HEARTS, SPADES
};

enum class Rank : uint8_t {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
};

struct Card {
    Suit suit;
    Rank rank;

    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {};

    [[nodiscard]] std::string toString() const;
};
} // namespace holdem

#endif //OPENHOLDEM_CARD_H