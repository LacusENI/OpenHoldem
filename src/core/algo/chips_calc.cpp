#include "chips_calc.h"

#include <algorithm>

#include "GameTypes.hpp"

namespace poker::algo {
struct BettingAction {
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
    chips_t target_bet;
    bool can_raise = true;

    chips_t chipsCommitted() const { return target_bet - player_bet; }
    chips_t raiseDelta() const { return target_bet - round_bet; }
    chips_t minRaise() const { return round_bet + min_raise_delta; }
    bool isAllIn() const { return chipsCommitted() == player_chips; }
    bool isChipsEnough() const { return chipsCommitted() <= player_chips; }
    bool isIncompleteCall() const { return target_bet < round_bet; }
    bool isCompleteCall() const { return target_bet == round_bet; }
    bool isIncompleteRaise() const { return target_bet > round_bet && target_bet < minRaise(); }
    bool isCompleteRaise() const { return target_bet >= minRaise(); }
};

Action get_action(BettingAction betting) {
    using enum Action;
    // 仅"不足额全下"是动作层面的特殊：不足跟注（Undercall）与不足最小加注（ShortAllIn）
    if (betting.isAllIn()) {
        if (betting.isIncompleteCall()) return Undercall;
        if (betting.isIncompleteRaise()) return betting.can_raise ? ShortAllIn : Void;
    }
    if (betting.isCompleteCall()) {
        return betting.player_bet == betting.round_bet ? Check : Call;
    }
    if (betting.isCompleteRaise() && betting.can_raise) {
        return betting.round_bet == 0 ? Bet : Raise;
    }
    return Void;
}

BettingResult process_betting(BettingAction betting) {
    BettingResult result;
    result.bet_delta = betting.chipsCommitted();
    result.player_bet = betting.target_bet;
    result.round_bet = std::max(betting.round_bet, betting.target_bet);
    result.min_raise_delta = std::max(betting.min_raise_delta, betting.raiseDelta());
    result.action = get_action(betting);
    result.is_target_bet_valid = result.action != Action::Void;
    result.has_sufficient_chips = betting.isChipsEnough();
    return result;
}

BettingResult process_betting(BettingContext context, BettingInput input) {
    BettingAction betting = {
        .player_chips = context.player_chips,
        .player_bet = context.player_bet,
        .round_bet = context.round_bet,
        .min_raise_delta = context.min_raise_delta,
        .target_bet = input.target_bet,
        .can_raise = context.can_raise,
    };
    BettingResult result = process_betting(betting);
    if (input.is_fold) result.action = Action::Fold;
    return result;
}

std::vector<chips_t> calculate_awards(int pot, int winners_n) {
    // 防御：没有赢家时不应进入分配，避免整数除零
    if (winners_n <= 0) return {};

    const int share = pot / winners_n;
    const int remainder = pot % winners_n;

    std::vector<chips_t> amounts;
    for (int i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        amounts.push_back(amount);
    }
    return amounts;
}
} // namespace poker::algo