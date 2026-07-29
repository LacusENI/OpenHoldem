#pragma once
#include <cstdint>
#include <sstream>
#include <string>

#include "GameTypes.hpp"

namespace poker {
enum class Rank : uint8_t;
enum class Suit : uint8_t;
struct Card;
enum class HandType;
class HandValue;
enum class Action;
enum class Stage;
struct Player;
enum class PlayerStatus;
} // namespace poker

namespace poker::ui::formatter {
std::string format(Rank rank);

std::string format(Suit suit);

std::string format(const Card& card);

std::string format(const HandType& hand_type);

std::string format(const HandValue& hand_value);

std::string format(const Action& action);

std::string format(const Stage& stage);

std::string format(const PlayerStatus& player_state);

template <std::size_t N>
std::string format(const std::array<Card, N>& cards) {
    std::stringstream ss;
    for (const Card& card : cards) {
        ss << format(card) << " ";
    }
    std::string result = ss.str();
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}
} // namespace poker::ui::formatter