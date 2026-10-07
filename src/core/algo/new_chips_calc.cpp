#include "new_chips_calc.h"

#include <algorithm>

namespace poker::algo {
namespace {
template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

struct Params {
    chips_t chips;
    chips_t bet;
    chips_t rb;  // round_bet
    chips_t mrd; // min_raise_delta
    bool can_raise;
};

NewBettingResult invalidResult() {
    return NewBettingResult{}; // kind == Invalid
}

/* 依据"本次投入 commit"结算出行动后的状态 */
NewBettingResult settle(const Params& p, NewActionKind kind, chips_t commit) {
    NewBettingResult result;
    result.kind = kind;
    result.bet_delta = commit;
    result.player_bet = p.bet + commit;
    result.is_all_in = (commit == p.chips);
    if (result.player_bet > p.rb) {
        result.round_bet = result.player_bet;
        result.min_raise_delta = std::max(p.mrd, result.player_bet - p.rb);
    } else {
        result.round_bet = p.rb;
        result.min_raise_delta = p.mrd;
    }
    return result;
}

/* 按"匹配到 round_bet"处理 Call（含不足额则 all-in） */
NewBettingResult call(const Params& p) {
    chips_t need = std::max<chips_t>(p.rb - p.bet, 0);
    chips_t commit = std::min(need, p.chips);
    if (need > p.chips) {
        // 筹码不足以跟平 -> 全下（不足额跟注）
        return settle(p, NewActionKind::AllIn, commit);
    }
    return settle(p, NewActionKind::Call, commit);
}

NewBettingResult allIn(const Params& p) {
    chips_t target = p.bet + p.chips;
    if (target > p.rb && !p.can_raise) return invalidResult(); // 不能加注
    return settle(p, NewActionKind::AllIn, p.chips);
}

NewBettingResult raiseBy(const Params& p, chips_t increment) {
    if (increment == 0) return call(p);       // increment==0 -> Call
    if (increment < 0) return invalidResult();

    chips_t target = p.rb + increment;
    chips_t commit = target - p.bet;
    if (commit < 0) return invalidResult();

    if (commit >= p.chips) {
        // 想加注但筹码不足或刚好全下 -> AllIn（无论是否达最小加注）
        return allIn(p);
    }
    if (!p.can_raise) return invalidResult();          // 想加注但被禁止
    if (target < p.rb + p.mrd) return invalidResult(); // 未达最小加注且非全下
    return settle(p, NewActionKind::Raise, commit);
}
} // namespace

NewBettingResult new_process_betting(const NewBettingContext& context,
                                     const NewPlayerAction& action) {
    Params p{context.player_chips, context.player_bet, context.round_bet, context.min_raise_delta,
             context.can_raise};
    return std::visit(Overloaded{
                          [&](FoldAction) { return settle(p, NewActionKind::Fold, 0); },
                          [&](CallAction) { return call(p); },
                          [&](AllInAction) { return allIn(p); },
                          [&](RaiseAction r) { return raiseBy(p, r.increment); },
                      },
                      action);
}

NewActionKind new_get_action(const NewBettingContext& context, const NewPlayerAction& action) {
    return new_process_betting(context, action).kind;
}
} // namespace poker::algo
