#include "chips_calc.h"

#include <gtest/gtest.h>

using namespace poker;
using namespace poker::algo;

TEST(ProcessBettingTest, Bet) {
    BettingContext context = {
        .player_chips = 100,
        .player_bet = 0,
        .round_bet = 0,
        .min_raise_delta = 0,
    };
    BettingInput input = {
        .target_bet = 20,
        .is_fold = false,
    };
    BettingResult result = process_betting(context, input);
    EXPECT_EQ(result.action, Action::BET);
    EXPECT_EQ(result.bet_delta, 20);
    EXPECT_EQ(result.player_bet, 20);
    EXPECT_EQ(result.round_bet, 20);
    EXPECT_EQ(result.min_raise_delta, 20);
    EXPECT_EQ(result.is_target_bet_valid, true);
    EXPECT_EQ(result.has_sufficient_chips, true);
}

TEST(ProcessBettingTest, Raise) {
    BettingContext context = {
        .player_chips = 100,
        .player_bet = 10,
        .round_bet = 30,
        .min_raise_delta = 20,
    };
    BettingInput input = {
        .target_bet = 60,
        .is_fold = false,
    };
    BettingResult result = process_betting(context, input);
    EXPECT_EQ(result.action, Action::RAISE);
    EXPECT_EQ(result.bet_delta, 50);
    EXPECT_EQ(result.player_bet, 60);
    EXPECT_EQ(result.round_bet, 60);
    EXPECT_EQ(result.min_raise_delta, 30);
    EXPECT_EQ(result.is_target_bet_valid, true);
    EXPECT_EQ(result.has_sufficient_chips, true);
}

TEST(ProcessBettingTest, Call) {
    BettingContext context = {
        .player_chips = 100,
        .player_bet = 20,
        .round_bet = 50,
        .min_raise_delta = 30,
    };
    BettingInput input = {
        .target_bet = 50,
        .is_fold = false,
    };
    BettingResult result = process_betting(context, input);
    EXPECT_EQ(result.action, Action::CALL);
    EXPECT_EQ(result.bet_delta, 30);
    EXPECT_EQ(result.player_bet, 50);
    EXPECT_EQ(result.round_bet, 50);
    EXPECT_EQ(result.min_raise_delta, 30);
    EXPECT_EQ(result.is_target_bet_valid, true);
    EXPECT_EQ(result.has_sufficient_chips, true);
}

TEST(ProcessBettingTest, Check) {
    BettingContext context = {
        .player_chips = 100,
        .player_bet = 10,
        .round_bet = 10,
        .min_raise_delta = 10,
    };
    BettingInput input = {.target_bet = 10, .is_fold = false};
    BettingResult result = process_betting(context, input);
    EXPECT_EQ(result.action, Action::CHECK);
    EXPECT_EQ(result.bet_delta, 0);
    EXPECT_EQ(result.player_bet, 10);
    EXPECT_EQ(result.round_bet, 10);
    EXPECT_EQ(result.min_raise_delta, 10);
    EXPECT_EQ(result.is_target_bet_valid, true);
    EXPECT_EQ(result.has_sufficient_chips, true);
}

TEST(ProcessBettingTest, Fold) {
    BettingContext context = {
        .player_chips = 100,
        .player_bet = 20,
        .round_bet = 70,
        .min_raise_delta = 20,
    };
    BettingInput input = {
        .target_bet = 0,
        .is_fold = true,
    };
    BettingResult result = process_betting(context, input);
    EXPECT_EQ(result.action, Action::FOLD);
}
