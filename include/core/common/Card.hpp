#pragma once

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <string>

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
    NONE = 0,
    TWO = 2,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE
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
    explicit Card(std::string_view str) { *this = from_string(str).value_or(Card{}); }
    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}

    bool operator<(const Card& other) const {
        if (rank == other.rank) return suit < other.suit;
        return rank < other.rank;
    }

    bool operator==(const Card& other) const { return suit == other.suit && rank == other.rank; }

    bool operator>(const Card& other) const {
        if (rank == other.rank) return suit > other.suit;
        return rank > other.rank;
    }

    static std::optional<Card> from_string(std::string_view str) {
        if (str.size() < 2) return std::nullopt;
        Suit s;
        switch (str[0]) {
        case 'C': s = Suit::CLUB; break;
        case 'D': s = Suit::DIAMOND; break;
        case 'H': s = Suit::HEART; break;
        case 'S': s = Suit::SPADE; break;
        case '_': s = Suit::NONE; break;
        default: return std::nullopt;
        }
        Rank r;
        switch (str[1]) {
        case 'T': r = Rank::TEN; break;
        case 'J': r = Rank::JACK; break;
        case 'Q': r = Rank::QUEEN; break;
        case 'K': r = Rank::KING; break;
        case 'A': r = Rank::ACE; break;
        case '_': r = Rank::NONE; break;
        default:
            if (str[1] >= '2' && str[1] <= '9')
                r = static_cast<Rank>(str[1] - '0');
            else
                return std::nullopt;
        }
        return Card{s, r};
    }
};

using cards_7_t = std::array<Card, 7>;
using cards_2_t = std::array<Card, 2>;
using cards_5_t = std::array<Card, 5>;

inline std::string to_string(Suit suit) {
    return std::string{magic_enum::enum_name(suit)};
}

inline std::string to_string(Rank rank) {
    return std::string{magic_enum::enum_name(rank)};
}

inline std::string to_string(Card card) {
    static std::vector<std::string> suits = {"_", "C", "D", "H", "S"};
    static std::vector<std::string> ranks = {"_", "?", "2", "3", "4", "5", "6", "7",
                                             "8", "9", "T", "J", "Q", "K", "A"};
    return suits[static_cast<int>(card.suit)] + ranks[static_cast<int>(card.rank)];
}

template <std::size_t N>
std::string to_string(const std::array<Card, N>& cards) {
    std::string result = "[";
    for (const auto& card : cards) {
        result += to_string(card) + ",";
    }
    if (!cards.empty()) result.pop_back();
    result += "]";
    return result;
}

template <typename T>
std::optional<T> from_string(std::string_view str);

template <>
inline std::optional<Card> from_string(std::string_view str) {
    return Card::from_string(str);
}

template <>
inline std::optional<Suit> from_string(std::string_view str) {
    return magic_enum::enum_cast<Suit>(str);
}

template <>
inline std::optional<Rank> from_string(std::string_view str) {
    return magic_enum::enum_cast<Rank>(str);
}
} // namespace poker
