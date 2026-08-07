#pragma once

#include <string>

#include <magic_enum/magic_enum.hpp>

#include "Card.hpp"
#include "GameTypes.hpp"

namespace poker {
/**
 * @brief 玩家实体类
 */
class Player {
public:
    enum class State : uint8_t { ACTIVE, ALL_IN, FOLDED, OUT };

    Player() = default;

    State getState() const { return state; }
    void setState(State new_state) { state = new_state; }
    cards_2_t getHoleCards() const { return hole_cards; }
    void setHoleCards(cards_2_t cards) { hole_cards = cards; }
    chips_t getChips() const { return chips; }
    void setChips(chips_t new_chips) { chips = new_chips; }
    chips_t getBet() const { return bet; }
    void setBet(chips_t new_bet) { bet = new_bet; }

    /* 玩家获取筹码 */
    void winChips(chips_t delta) { chips += delta; }
    /* 玩家支付筹码 */
    void commitChips(chips_t delta) { chips -= delta; }
    bool isActive() const { return state == State::ACTIVE; }
    bool isAllIn() const { return state == State::ALL_IN; }
    bool isFolded() const { return state == State::FOLDED; }
    bool isOut() const { return state == State::OUT; }
    bool isAlive() const { return state == State::ACTIVE || state == State::ALL_IN; }

    /* 玩家弃牌 */
    void fold() { state = State::FOLDED; }
    /* 玩家 All-in */
    void allIn() { state = State::ALL_IN; }
    /* 玩家出局 */
    void out() { state = State::OUT; }

private:
    State state = State::ACTIVE; // 玩家状态

    cards_2_t hole_cards{}; // 玩家的底牌

    chips_t chips = 0; // 玩家拥有的筹码
    chips_t bet = 0;   // 玩家的下注额
};

template <typename T>
std::optional<T> from_string(std::string_view str);

inline std::string to_string(Player::State state) {
    return std::string{magic_enum::enum_name(state)};
}

template <>
inline std::optional<Player::State> from_string(std::string_view str) {
    return magic_enum::enum_cast<Player::State>(str);
}

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

inline std::string to_string(PlayerStatus status) {
    return std::string{magic_enum::enum_name(status)};
}

template <>
inline std::optional<PlayerStatus> from_string(std::string_view str) {
    return magic_enum::enum_cast<PlayerStatus>(str);
}

} // namespace poker
