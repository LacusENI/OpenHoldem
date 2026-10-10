#pragma once

#include <variant>

#include "GameTypes.hpp"

namespace poker::betting {
/**
 * @brief 意图（Command）：玩家能表达的扑克动作
 *
 * 与"结果"共用同一套词汇（Fold/Check/Call/Bet/Raise/AllIn）；规则推导出的
 * "不足额 / 全下 / 是否重开加注"等不再作为动作种类，而是结果里的标志。
 *
 * - Check / Call：有无待跟注额（round_bet）的区分
 * - Bet / Raise ：有无前置下注的区分；Raise 的 amount 是增量(= target_bet - round_bet)
 * - AllIn       ：所有 all-in（不足额跟注 / 跟平 / 短加注 / 完整加注）的唯一入口
 *
 * 合法性（显式意图，不做隐式改写）：
 * - Bet/Raise 必须是合法的"完整"下注/加注：低于最小额 -> BelowMinRaise；
 *   所需额度超过筹码 -> InsufficientChips。短全下/全下请改用 AllIn。
 * - 结果里用标志区分"不足额(is_short)/全下(is_all_in)/是否重开(reopens_betting)"。
 */
struct Fold {};
struct AllIn {};
struct Check {};
struct Call {};
struct Bet {
    chips_t bet_amount; // 开注额
};
struct Raise {
    chips_t raise_by; // 加注额
};
using Intent = std::variant<AllIn, Bet, Call, Check, Fold, Raise>;

struct Context {
    chips_t player_chips;  // 玩家现有筹码数量
    chips_t player_bet;    // 玩家已下注的额度
    chips_t round_bet;     // 回合最高下注额
    chips_t min_add_on;    // 最小加注额
    bool can_raise = true; // 玩家是否可以加注
};

/* 玩家行动失败原因 */
enum class Error {
    CannotCheck,       // Check：有待跟注，应 Call
    NothingToCall,     // Call：无人下注，应 Check
    NoBetToRaise,      // Raise：无前置下注，应 Bet
    AlreadyBet,        // Bet：已有前置下注，应 Raise
    NonPositiveAmount, // Bet/Raise：金额/增量非正
    CannotRaise,       // Bet/Raise：不允许加注（或不足以加注）
    BelowMinRaise,     // Bet/Raise：未达最小加注（Bet/Raise 必须合法；短全下请用 AllIn）
    InsufficientChips, // Bet/Raise 所需额度超过筹码（应改用 AllIn）
};

/** 
 * @brief 玩家行动结果（成功时） 
 */
struct Success {
    Action action_kind; // 本次行动的类型
    chips_t player_bet; // 行动后本轮下注
    chips_t round_bet;  // 行动后的回合下注额
    chips_t min_add_on; // 行动后最小加注额

    bool is_short = false; // 是否是 short all-in
    bool is_full = false;  // 是否是完整加注/下注
};

using Result = std::variant<Success, Error>;

Result process(const Intent& intent, const Context& ctx);

} // namespace poker::algo
