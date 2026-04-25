#pragma once
#include <string>
#include <cstdint>

#include "GameTypes.h"

namespace holdem {
enum class Rank : uint8_t;
enum class Suit : uint8_t;
class Card;
enum class HandType;
class HandValue;
enum class ActionType;
class Action;
enum class GameState;
class Player;
}

namespace holdem::ui::formatter {
std::string format(Rank rank);

std::string format(Suit suit);

std::string format(const Card& card);

std::string format(const HandType& hand_type);

std::string format(const HandValue& hand_value);

std::string format(const ActionType& action_type);

std::string format(const GameState& state);

std::string format(const Position& position);
}