#include <gtest/gtest.h>

#include "GameTypes.hpp"
#include "new_chips_calc.h"

using namespace poker;
using namespace poker::algo;

namespace {
NewBettingContext ctx(chips_t chips, chips_t bet, chips_t round_bet, chips_t min_raise_delta,
                      bool can_raise = true) {
    return NewBettingContext{chips, bet, round_bet, min_raise_delta, can_raise};
}
} // namespace

// Fold
TEST(NewChipsCalcTest, Fold) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10), FoldAction{});
    EXPECT_EQ(NewActionKind::Fold, r.kind);
    EXPECT_EQ(0, r.bet_delta);
    EXPECT_FALSE(r.is_all_in);
}

// Call 在无人下注时即过牌(Check)
TEST(NewChipsCalcTest, CallAsCheck) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 0, 0), CallAction{});
    EXPECT_EQ(NewActionKind::Call, r.kind);
    EXPECT_EQ(0, r.bet_delta);
    EXPECT_EQ(0, r.player_bet);
    EXPECT_EQ(0, r.round_bet);
}

// Call 跟平
TEST(NewChipsCalcTest, CallFull) {
    NewBettingContext c = ctx(100, 0, 10, 10);
    NewBettingResult r = new_process_betting(c, CallAction{});
    EXPECT_EQ(NewActionKind::Call, r.kind);
    EXPECT_EQ(10, r.bet_delta);
    EXPECT_EQ(10, r.player_bet);
    EXPECT_EQ(10, r.round_bet);
}

// Call 但筹码不足 -> 全下（不足额跟注）
TEST(NewChipsCalcTest, CallUnderStack) {
    NewBettingResult r = new_process_betting(ctx(5, 0, 10, 10), CallAction{});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(5, r.bet_delta);
    EXPECT_TRUE(r.is_all_in);
    EXPECT_EQ(10, r.round_bet); // 不影响 round_bet
}

// AllIn 不足额跟注
TEST(NewChipsCalcTest, AllInUnderCall) {
    NewBettingResult r = new_process_betting(ctx(5, 0, 10, 10), AllInAction{});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(5, r.bet_delta);
    EXPECT_EQ(5, r.player_bet);
    EXPECT_EQ(10, r.round_bet);
    EXPECT_TRUE(r.is_all_in);
}

// AllIn 恰好跟平
TEST(NewChipsCalcTest, AllInCall) {
    NewBettingResult r = new_process_betting(ctx(10, 0, 10, 10), AllInAction{});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(10, r.bet_delta);
    EXPECT_EQ(10, r.player_bet);
}

// AllIn 完整加注
TEST(NewChipsCalcTest, AllInCompleteRaise) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10), AllInAction{});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(100, r.player_bet);
    EXPECT_EQ(100, r.round_bet);
    EXPECT_EQ(90, r.min_raise_delta); // 100 - 10
}

// AllIn 不足额加注（short all-in raise）
TEST(NewChipsCalcTest, AllInShortRaise) {
    NewBettingResult r = new_process_betting(ctx(15, 0, 10, 10), AllInAction{});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(15, r.player_bet);
    EXPECT_EQ(15, r.round_bet);
    EXPECT_EQ(10, r.min_raise_delta); // 15-10=5 < 10，保持
}

// AllIn 但超出跟注额且不允许加注 -> 非法
TEST(NewChipsCalcTest, AllInCannotRaise) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10, false), AllInAction{});
    EXPECT_FALSE(r.isValid());
    EXPECT_EQ(NewActionKind::Invalid, r.kind);
}

// Raise 正常加注
TEST(NewChipsCalcTest, RaiseNormal) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10), RaiseAction{30});
    EXPECT_EQ(NewActionKind::Raise, r.kind);
    EXPECT_EQ(40, r.player_bet); // rb + increment
    EXPECT_EQ(40, r.round_bet);
    EXPECT_EQ(30, r.min_raise_delta);
}

// Raise 作为首次下注（round_bet == 0）
TEST(NewChipsCalcTest, RaiseAsBet) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 0, 0), RaiseAction{20});
    EXPECT_EQ(NewActionKind::Raise, r.kind);
    EXPECT_EQ(20, r.player_bet);
    EXPECT_EQ(20, r.round_bet);
    EXPECT_EQ(20, r.min_raise_delta);
}

// Raise(increment == 0) 归一化为 Call
TEST(NewChipsCalcTest, RaiseZeroIncrementIsCall) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10), RaiseAction{0});
    EXPECT_EQ(NewActionKind::Call, r.kind);
    EXPECT_EQ(10, r.bet_delta);
}

// Raise 会投入全部筹码 -> 归一化为 AllIn
TEST(NewChipsCalcTest, RaiseBecomesAllIn) {
    NewBettingResult r = new_process_betting(ctx(30, 0, 10, 10), RaiseAction{50});
    EXPECT_EQ(NewActionKind::AllIn, r.kind);
    EXPECT_EQ(30, r.player_bet);
    EXPECT_EQ(30, r.round_bet);
    EXPECT_TRUE(r.is_all_in);
}

// 非全下的、低于最小加注的加注 -> 非法
TEST(NewChipsCalcTest, RaiseBelowMinIsInvalid) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10), RaiseAction{5});
    EXPECT_FALSE(r.isValid());
}

// 不允许加注时加注 -> 非法
TEST(NewChipsCalcTest, RaiseCannotRaiseIsInvalid) {
    NewBettingResult r = new_process_betting(ctx(100, 0, 10, 10, false), RaiseAction{30});
    EXPECT_FALSE(r.isValid());
}

// new_get_action 返回归一化动作
TEST(NewChipsCalcTest, GetActionKind) {
    EXPECT_EQ(NewActionKind::Call,
              new_get_action(ctx(100, 0, 10, 10), RaiseAction{0}));
    EXPECT_EQ(NewActionKind::AllIn,
              new_get_action(ctx(30, 0, 10, 10), RaiseAction{50}));
    EXPECT_EQ(NewActionKind::Raise,
              new_get_action(ctx(100, 0, 10, 10), RaiseAction{30}));
}
