#pragma once

#include <string>

#include <magic_enum/magic_enum.hpp>

#include "Card.hpp"
#include "GameTypes.hpp"

namespace poker {
enum class PlayerStatus {
    IDLE,
    BET,
    RAISED,
    CALLED,
    CHECKED,
    ALL_IN,
    FOLDED,
    SMALL_BLIND,
    BIG_BLIND
};

inline std::string to_string(PlayerStatus state) {
    return std::string{magic_enum::enum_name(state)};
}

template <typename T>
std::optional<T> from_string(std::string_view str);

template <>
inline std::optional<PlayerStatus> from_string(std::string_view str) {
    return magic_enum::enum_cast<PlayerStatus>(str);
}

/**
 * @brief 玩家实体类
 */
struct Player {
    seat_idx_t position;  // 玩家所在位置
    cards_2_t hole_cards; // 玩家的底牌

    chips_t chips = 0;      // 玩家拥有的筹码
    chips_t bet = 0;        // 玩家的下注额
    bool is_folded = false; // 玩家是否已弃牌
    bool is_allin = false;  // 玩家是否已 all-in

    void winChips(chips_t delta) { chips += delta; }
    void commitChips(chips_t delta) { chips -= delta; }
    /* 玩家是否在场(未弃牌) */
    bool isActive() const { return !is_folded; }
    /* 玩家是否能继续行动(未弃牌也未 all-in) */
    bool canAct() const { return !is_folded && !is_allin; }
};

} // namespace poker
