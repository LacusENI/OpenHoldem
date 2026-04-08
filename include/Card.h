#ifndef OPENHOLDEM_CARD_H
#define OPENHOLDEM_CARD_H
#include <cstdint>
#include <string>
#include <array>

namespace holdem {

/**
 * @brief 扑克牌的花色
 */
enum class Suit : uint8_t {
    CLUB,    // 梅花
    DIAMOND, // 方块
    HEART,   // 红心
    SPADE,   // 黑桃
};

/**
 * @brief 扑克牌的点数 (2-10, J, Q, K, A)
 */
enum class Rank : uint8_t {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
};

/**
 * @brief 扑克牌的实体类
 */
struct Card {
    Suit suit;
    Rank rank;

    Card() : suit(), rank() {}
    explicit Card(const char* str);
    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

    /* 用于输出到日志中的展示字符串 */
    std::string toString() const;
    /* 用于输出到用户界面的展示字符串 */
    std::string toMessage() const;

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

using Cards7 = std::array<Card, 7>;
using Cards2 = std::array<Card, 2>;
using Cards5 = std::array<Card, 5>;

/* 将2张牌和5张牌数组拼接成一个7张牌数组 */
Cards7 concatCards(const Cards2& cards2, const Cards5& cards5);

} // namespace holdem

#endif //OPENHOLDEM_CARD_H