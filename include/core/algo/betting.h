#pragma once

#include <cassert>
#include <variant>

#include "GameTypes.hpp"

namespace poker::betting {
struct Fold {};
struct AllIn {};
struct Check {};
struct Call {};
struct Bet {
    chips_t bet_to; // 开注额
};
struct Raise {
    chips_t raise_to; // 加注后的下注额
};
using Intent = std::variant<AllIn, Bet, Call, Check, Fold, Raise>;

struct Context {
    chips_t big_blind;     // 大盲注额
    chips_t player_chips;  // 玩家现有筹码数量
    chips_t player_bet;    // 玩家已下注的额度
    chips_t round_bet;     // 回合最高下注额
    chips_t min_raise_by;  // 最小加注额
    bool can_raise = true; // 玩家是否可以加注
};

/** 
 * @brief 玩家行动失败原因 
 * @details 错误判定顺序(优先级):
 * 1. 行动被禁止: CannotRaise
 * 2. 行动不满足前置条件: NothingToCall, NotBetToRaise, ...
 * 3. 行动不满足要求：BelowMinBet, BelowMinRaise
 * 4. 玩家没有能力支付行动所需: InsufficientChips
 */
enum class Error {
    CannotCheck,       // Check：有待跟注，应 Call
    NothingToCall,     // Call：无人下注，应 Check
    CannotRaise,       // Bet/Raise：不允许加注
    NoBetToRaise,      // Raise：无前置下注，应 Bet
    AlreadyBet,        // Bet：已有前置下注，应 Raise
    BelowMinBet,       // Bet: 未达最小下注（大盲注）
    BelowMinRaise,     // Raise：未达最小加注
    InsufficientChips, // Bet/Raise 所需额度超过筹码（应改用 AllIn）
};

/** 
 * @brief 玩家行动结果（成功时） 
 */
struct Success {
    Action action_kind;   // 本次行动的类型
    chips_t player_bet;   // 行动后本轮下注
    chips_t round_bet;    // 行动后的回合下注额
    chips_t min_raise_by; // 行动后最小加注额
};

using Result = std::variant<Success, Error>;

inline Result process(const Fold& _, const Context& ctx) {
    return Success{
        .action_kind = Action::Fold,
        .player_bet = ctx.player_bet,
        .round_bet = ctx.round_bet,
        .min_raise_by = ctx.min_raise_by,
    };
}

inline Result process(const Check& _, const Context& ctx) {
    if (ctx.player_bet != ctx.round_bet) {
        return Error::CannotCheck;
    }
    return Success{
        .action_kind = Action::Check,
        .player_bet = ctx.player_bet,
        .round_bet = ctx.round_bet,
        .min_raise_by = ctx.min_raise_by,
    };
}

inline Result process(const Call& _, const Context& ctx) {
    if (ctx.round_bet == 0) {
        return Error::NothingToCall;
    }
    chips_t amount = ctx.round_bet - ctx.player_bet;
    if (amount > ctx.player_chips) {
        return Error::InsufficientChips;
    }
    return Success{
        .action_kind = Action::Call,
        .player_bet = ctx.round_bet,
        .round_bet = ctx.round_bet,
        .min_raise_by = ctx.min_raise_by,
    };
}

inline Result process(const Bet& bet, const Context& ctx) {
    if (ctx.round_bet > 0) {
        return Error::AlreadyBet;
    }
    if (bet.bet_to < ctx.big_blind) {
        return Error::BelowMinBet;
    }
    chips_t amount = bet.bet_to - ctx.player_bet;
    if (amount > ctx.player_chips) {
        return Error::InsufficientChips;
    }
    return Success{
        .action_kind = Action::Bet,
        .player_bet = bet.bet_to,
        .round_bet = bet.bet_to,
        .min_raise_by = bet.bet_to,
    };
}

inline Result process(const Raise& raise, const Context& ctx) {
    if (!ctx.can_raise) {
        return Error::CannotRaise;
    }
    if (ctx.round_bet == 0) {
        return Error::NoBetToRaise;
    }
    chips_t raise_by = raise.raise_to - ctx.round_bet;
    if (raise_by < ctx.min_raise_by) {
        return Error::BelowMinRaise;
    }
    chips_t amount = raise.raise_to - ctx.player_bet;
    if (amount > ctx.player_chips) {
        return Error::InsufficientChips;
    }
    return Success{
        .action_kind = Action::Raise,
        .player_bet = raise.raise_to,
        .round_bet = raise.raise_to,
        .min_raise_by = raise_by,
    };
}

inline Result process(const AllIn& _, const Context& ctx) {
    chips_t intended_bet = ctx.player_bet + ctx.player_chips;
    if (intended_bet < ctx.round_bet) {
        return Success{
            .action_kind = Action::Undercall,
            .player_bet = intended_bet,
            .round_bet = ctx.round_bet,
            .min_raise_by = ctx.min_raise_by,
        };
    }
    if (intended_bet == ctx.round_bet) {
        return Success{
            .action_kind = Action::Call,
            .player_bet = ctx.round_bet,
            .round_bet = ctx.round_bet,
            .min_raise_by = ctx.min_raise_by
        };
    }
    chips_t raise_by = intended_bet - ctx.round_bet;
    if (raise_by < ctx.min_raise_by) {
        return Success{
            .action_kind = Action::ShortAllIn,
            .player_bet = intended_bet,
            .round_bet = intended_bet,
            .min_raise_by = ctx.min_raise_by,
        };
    }
    if (!ctx.can_raise) {
        return Error::CannotRaise; 
    }
    Action action_kind = ctx.round_bet == 0 ? Action::Bet : Action::Raise;
    return Success{
        .action_kind = action_kind,
        .player_bet = intended_bet,
        .round_bet = intended_bet,
        .min_raise_by = raise_by,
    };
}

inline Result process(const Intent& intent, const Context& ctx) {
    return std::visit([&ctx](const auto& param) {
        return process(param, ctx);
    }, intent);
}

} // namespace poker::betting