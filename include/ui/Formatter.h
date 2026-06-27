#pragma once
#include <string>
#include <cstdint>

#include "GameTypes.h"

namespace poker {
enum class Rank : uint8_t;
enum class Suit : uint8_t;
class Card;
enum class HandType;
class HandValue;
enum class Action;
enum class Stage;
struct Player;
}

namespace poker::ui::formatter {
std::string format(Rank rank);

std::string format(Suit suit);

std::string format(const Card& card);

std::string format(const HandType& hand_type);

std::string format(const HandValue& hand_value);

std::string format(const Action& action);

std::string format(const Stage& stage);

std::string format(const seat_idx_t& position);
}