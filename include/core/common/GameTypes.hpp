#pragma once

#include <string>

#include <magic_enum/magic_enum.hpp>

namespace poker {
using seat_t = int;  // 玩家座位
using chips_t = int; // 筹码数量

enum class Stage { IDLE, PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, SETTLEMENT };

enum class Action { VOID, CALL, CHECK, RAISE, BET, FOLD, ALL_IN };

inline Stage next_stage(Stage stage) {
    return static_cast<Stage>(static_cast<int>(stage) + 1);
}

inline std::string to_string(Stage stage) {
    return std::string{magic_enum::enum_name(stage)};
}

inline std::string to_string(Action action) {
    return std::string{magic_enum::enum_name(action)};
}

template <typename T>
std::optional<T> from_string(std::string_view str);

template <>
inline std::optional<Stage> from_string(std::string_view str) {
    return magic_enum::enum_cast<Stage>(str);
}

template <>
inline std::optional<Action> from_string(std::string_view str) {
    return magic_enum::enum_cast<Action>(str);
}
} // namespace poker
