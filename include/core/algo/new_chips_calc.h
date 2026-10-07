#pragma once

#include <variant>

#include "GameTypes.hpp"

namespace poker::algo {
/**
 * @brief 显式玩家行动（实验版）：以"意图"表达，而非目标下注额
 *
 * - Call : 匹配当前下注；若当前无人下注（round_bet 为 0）即过牌(Check)
 *          —— 领域里 Call/Check 没有统一的合称；多数接口就用 Call 表示"匹配/过牌"。
 * - Raise: 相对当前 round_bet 的增量（increment = target_bet - round_bet）；
 *          首次下注（round_bet==0）也用它
 *          —— Raise/Bet 的统一术语是 "aggression"（TDA 用 "the last aggressive action"）。
 *
 * 归一化规则：
 * - Raise(increment==0) 归一化为 Call；
 * - Raise/AllIn 若实际会投入全部筹码，一律视为 AllIn（无论是否达到最小加注）。
 */
struct FoldAction {};
struct AllInAction {};
struct CallAction {};
struct RaiseAction {
    chips_t increment; // target_bet - round_bet
};
using NewPlayerAction = std::variant<FoldAction, AllInAction, CallAction, RaiseAction>;

struct NewBettingContext {
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
    bool can_raise = true;
};

/* 归一化后的实际动作（Invalid 表示该意图非法） */
enum class NewActionKind { Invalid, Fold, Call, Raise, AllIn };

struct NewBettingResult {
    NewActionKind kind = NewActionKind::Invalid;
    chips_t bet_delta = 0;       // 本次实际投入
    chips_t player_bet = 0;      // 行动后本轮下注
    chips_t round_bet = 0;       // 行动后 round_bet
    chips_t min_raise_delta = 0; // 行动后最小加注增量
    bool is_all_in = false;      // 是否因此 all-in

    [[nodiscard]] bool isValid() const { return kind != NewActionKind::Invalid; }
};

/* 只求归一化后的动作类型 */
NewActionKind new_get_action(const NewBettingContext& context, const NewPlayerAction& action);

/* 处理一次玩家行动，返回行动后的下注状态 */
NewBettingResult new_process_betting(const NewBettingContext& context,
                                     const NewPlayerAction& action);
} // namespace poker::algo
