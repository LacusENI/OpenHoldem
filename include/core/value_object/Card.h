#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace holdem {

/**
 * @brief 扑克牌的花色
 */
enum class Suit : uint8_t {
    NONE = 0,
    CLUB,    // 梅花
    DIAMOND, // 方块
    HEART,   // 红心
    SPADE,   // 黑桃
};

/**
 * @brief 扑克牌的点数 (2-10, J, Q, K, A)
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

    /* 用于输出到日志中的展示字符串 */
    std::string toString() const;

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

std::ostream& operator<<(std::ostream& os, const Card& card);

using Cards7 = std::array<Card, 7>;
using Cards2 = std::array<Card, 2>;
using Cards5 = std::array<Card, 5>;

/* 将2张牌和5张牌数组拼接成一个7张牌数组 */
Cards7 concatCards(const Cards2& cards2, const Cards5& cards5);

namespace utils {
std::string castSuitToString(Suit suit);
std::string castRankToString(Rank rank);
}
} // namespace holdem
