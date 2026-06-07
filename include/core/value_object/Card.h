#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace poker {
/**
 * @brief 扑克牌的花色 (Club/Diamond/heart/spade)
 */
enum class Suit : uint8_t {
    NONE = 0,
    CLUB,    // 梅花
    DIAMOND, // 方块
    HEART,   // 红心
    SPADE,   // 黑桃
};

/**
 * @brief 扑克牌的点数 (2~10/Jack/Queen/King/Ace)
 */
enum class Rank : uint8_t {
    NONE = 0, TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
};

constexpr int MIN_SUIT = static_cast<int>(Suit::CLUB);
constexpr int MAX_SUIT = static_cast<int>(Suit::SPADE);
constexpr int MIN_RANK = static_cast<int>(Rank::TWO);
constexpr int MAX_RANK = static_cast<int>(Rank::ACE);

/**
 * @brief 扑克牌的实体类
 */
struct Card {
    Suit suit;
    Rank rank;

    Card() : suit(Suit::NONE), rank(Rank::NONE) {}
    explicit Card(const char* str);
    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

    bool operator<(const Card& other) const {
        if (rank == other.rank) return suit < other.suit;
        return rank < other.rank;
    }

    bool operator==(const Card& other) const {
        return suit == other.suit && rank == other.rank;
    }

    bool operator>(const Card& other) const {
        if (rank == other.rank) return suit > other.suit;
        return rank > other.rank;
    }
};

using cards_7_t = std::array<Card, 7>;
using cards_2_t = std::array<Card, 2>;
using cards_5_t = std::array<Card, 5>;

std::string to_string(Suit suit);
std::string to_string(Rank rank);
std::string to_string(Card card);
std::ostream& operator<<(std::ostream& os, Suit suit);
std::ostream& operator<<(std::ostream& os, Rank rank);
std::ostream& operator<<(std::ostream& os, Card card);
} // namespace holdem
