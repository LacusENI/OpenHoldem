#pragma once

#include <cstdint>
#include <string>

#include <magic_enum/magic_enum.hpp>

namespace poker {
using seat_t = int;  // 玩家座位
using chips_t = int; // 筹码数量
using client_id_t = std::uint32_t; // 连接 / 客户端标识（join 前即可用）

/* 无效座位/行动者标识 */
inline constexpr seat_t INVALID_SEAT = -1;

/* 无效客户端标识（有效 id 从 1 开始） */
inline constexpr client_id_t INVALID_CLIENT_ID = 0;

enum class Stage { IDLE, PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, SETTLEMENT };

enum class Action {
    Void,
    Fold,
    Check,
    Call,
    Bet,
    Raise,
    Undercall,  // 全下但不足跟注额
    ShortAllIn  // 全下但未达到最小加注额
};

/* 下一阶段；已到末尾（SETTLEMENT）时保持不变，避免产生越界的 Stage 值 */
inline Stage next_stage(Stage stage) {
    const int next = static_cast<int>(stage) + 1;
    if (next > static_cast<int>(Stage::SETTLEMENT)) return Stage::SETTLEMENT;
    if (next < static_cast<int>(Stage::IDLE)) return Stage::IDLE;
    return static_cast<Stage>(next);
}

/* 是否为翻牌前下注阶段 */
[[nodiscard]] inline bool isPreflop(Stage stage) {
    return stage == Stage::PREFLOP;
}

/* 是否为翻牌后下注阶段（仅 FLOP/TURN/RIVER，不含 IDLE/SHOWDOWN/SETTLEMENT） */
[[nodiscard]] inline bool isPostflop(Stage stage) {
    return stage == Stage::FLOP || stage == Stage::TURN || stage == Stage::RIVER;
}

inline std::string to_string(Stage stage) {
    return std::string{magic_enum::enum_name(stage)};
}

inline std::string to_string(Action action) {
    return std::string{magic_enum::enum_name(action)};
}
} // namespace poker
