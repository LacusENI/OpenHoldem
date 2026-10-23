#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <magic_enum/magic_enum.hpp>

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

inline constexpr std::optional<Suit> parse_suit(char str) {
    switch (str) {
    case 'c': return Suit::CLUB;
    case 'd': return Suit::DIAMOND;
    case 'h': return Suit::HEART;
    case 's': return Suit::SPADE;
    case '_': return Suit::NONE;
    default: return std::nullopt;
    }
}

inline constexpr std::optional<Rank> parse_rank(char str) {
    switch (str) {
    case '2': return Rank::TWO;
    case '3': return Rank::THREE;
    case '4': return Rank::FOUR;
    case '5': return Rank::FIVE;
    case '6': return Rank::SIX;
    case '7': return Rank::SEVEN;
    case '8': return Rank::EIGHT;
    case '9': return Rank::NINE;
    case 'T': return Rank::TEN;
    case 'J': return Rank::JACK;
    case 'Q': return Rank::QUEEN;
    case 'K': return Rank::KING;
    case 'A': return Rank::ACE;
    case '_': return Rank::NONE;
    default: return std::nullopt;
    }
}

/**
 * @brief 扑克牌的实体类
 */
struct Card {
    Rank rank;
    Suit suit;

    constexpr Card() : rank(Rank::NONE), suit(Suit::NONE) {}
    constexpr Card(Rank rank, Suit suit) : rank(rank), suit(suit) {}
    consteval Card(std::string_view str) {
        auto card = from_string(str);
        if (!card) throw std::invalid_argument("Invalid card string: " + std::string(str));
        rank = card->rank;
        suit = card->suit;
    }

    constexpr uint16_t pack() const {
        return (static_cast<uint16_t>(rank) << 8) | static_cast<uint16_t>(suit);
    }

    constexpr auto operator==(const Card& other) const { return pack() == other.pack(); }
    constexpr auto operator<=>(const Card& other) const { return pack() <=> other.pack(); }

    static constexpr std::optional<Card> from_string(std::string_view str) {
        if (str.size() != 2) return std::nullopt;
        auto rank = parse_rank(str[0]);
        auto suit = parse_suit(str[1]);
        if (!rank || !suit) return std::nullopt;
        return Card{*rank, *suit};
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
    static constexpr std::array<const char*, 5> suits = {"_", "c", "d", "h", "s"};
    static constexpr std::array<const char*, 15> ranks = {"_", "?", "2", "3", "4", "5", "6", "7",
                                                          "8", "9", "T", "J", "Q", "K", "A"};
    const int rank = static_cast<int>(card.rank);
    const int suit = static_cast<int>(card.suit);
    const char* rank_str =
        (rank >= 0 && rank < static_cast<int>(ranks.size())) ? ranks[rank] : "?";
    const char* suit_str =
        (suit >= 0 && suit < static_cast<int>(suits.size())) ? suits[suit] : "_";
    return std::string(rank_str) + suit_str;
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

} // namespace poker
