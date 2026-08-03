#include "eval_hand.h"

#include <gtest/gtest.h>

using namespace poker;
using namespace poker::algo;
using namespace poker::internal::eval_hand;

// 测试生成的组合的数量是否正确
TEST(EvalHandTest, CombinationSize) {
    cards_7_t hand7 = {Card("Ac"), Card("2h"), Card("8d"), Card("3c"),
                       Card("Jh"), Card("Kd"), Card("Qh")};
    auto combos = enum_5_from_7(hand7);

    EXPECT_EQ(combos.size(), 21);
}

// 测试生成的组合是否不重复
TEST(EvalHandTest, CombinationsUniqueness) {
    cards_7_t hand7 = {Card("Ac"), Card("2h"), Card("8d"), Card("3c"),
                       Card("Jh"), Card("Kd"), Card("Qh")};

    auto combos = enum_5_from_7(hand7);
    std::set<cards_5_t> uniqueCombos;
    for (auto combo : combos) {
        std::sort(combo.begin(), combo.end());
        uniqueCombos.insert(combo);
    }

    EXPECT_EQ(uniqueCombos.size(), 21);
}

// 测试点数频次统计是否正确
TEST(EvalHandTest, GetSortedCounts) {
    cards_5_t highCard = {Card("Ac"), Card("2h"), Card("8d"), Card("3c"),
                          Card("Jh")}; // A, 2, 8, 3, J -> A, J, 8, 3, 2
    auto resultHighCard = eval_hand_counts(highCard);
    ASSERT_EQ(resultHighCard.size(), 5);
    EXPECT_EQ(resultHighCard[0], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultHighCard[1], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultHighCard[2], RankCount(Rank::EIGHT, 1));
    EXPECT_EQ(resultHighCard[3], RankCount(Rank::THREE, 1));
    EXPECT_EQ(resultHighCard[4], RankCount(Rank::TWO, 1));

    cards_5_t twoPair = {Card("Ac"), Card("2h"), Card("3d"), Card("3c"),
                         Card("Jh")}; // A, 2, 3, 3, J -> 3, 3, A, J, 2
    auto resultTwoPair = eval_hand_counts(twoPair);
    ASSERT_EQ(resultTwoPair.size(), 4);
    EXPECT_EQ(resultTwoPair[0], RankCount(Rank::THREE, 2));
    EXPECT_EQ(resultTwoPair[1], RankCount(Rank::ACE, 1));
    EXPECT_EQ(resultTwoPair[2], RankCount(Rank::JACK, 1));
    EXPECT_EQ(resultTwoPair[3], RankCount(Rank::TWO, 1));

    cards_5_t fullHouse = {
        Card("8c"), Card("4h"), Card("8d"), Card("4c"), Card("4d"),
    }; // 8, 4, 8, 4, 4 -> 4, 4, 4, 8, 8
    auto resultFullHouse = eval_hand_counts(fullHouse);
    ASSERT_EQ(resultFullHouse.size(), 2);
    EXPECT_EQ(resultFullHouse[0], RankCount(Rank::FOUR, 3));
    EXPECT_EQ(resultFullHouse[1], RankCount(Rank::EIGHT, 2));
}

// 测试牌型判断是否正确
TEST(EvalHandTest, EvaluateHandType) {
    cards_5_t flush = {Card("Ah"), Card("2h"), Card("Jh"), Card("7h"),
                       Card("Kh")}; // 同花 A, 2, J, 7, K
    HandValue flushValue = eval_hand_value(flush);
    EXPECT_EQ(flushValue.getHandType(), HandType::FLUSH);

    cards_5_t fullHouse = {
        Card("8c"), Card("8h"), Card("5s"), Card("5d"), Card("5c"),
    }; // 葫芦 8, 8, 5, 5, 5
    HandValue fullHouseValue = eval_hand_value(fullHouse);
    EXPECT_EQ(fullHouseValue.getHandType(), HandType::FULL_HOUSE);

    cards_5_t straight = {
        Card("2c"), Card("Ah"), Card("4h"), Card("3d"), Card("5s"),
    }; // 顺子 2, A, 4, 3, 5
    HandValue straightValue = eval_hand_value(straight);
    EXPECT_EQ(straightValue.getHandType(), HandType::STRAIGHT);

    cards_5_t royalFlush = {Card("Ts"), Card("As"), Card("Js"), Card("Qs"),
                            Card("Ks")}; // 皇家同花顺
    HandValue royalFlushValue = eval_hand_value(royalFlush);
    EXPECT_EQ(royalFlushValue.getHandType(), HandType::ROYAL_FLUSH);
}

// 测试手牌分值计算是否正确
TEST(EvalHandTest, GetHandValue) {
    cards_5_t fullHouse = {Card("8c"), Card("8h"), Card("5s"), Card("5d"),
                           Card("5c")}; // 葫芦 8, 8, 5, 5, 5 -> 5, 5, 5, 8, 8
    HandValue fullHouseValue = eval_hand_value(fullHouse);
    EXPECT_EQ(fullHouseValue.getHandType(), HandType::FULL_HOUSE);
    EXPECT_EQ(fullHouseValue.getRank(0), Rank::FIVE);
    EXPECT_EQ(fullHouseValue.getRank(1), Rank::EIGHT);

    cards_5_t highCard = {Card("8c"), Card("9h"), Card("Ts"), Card("Qd"),
                          Card("3c")}; // 高牌 8, 9, 10, Q, 3 -> Q, 10, 9, 8, 3
    HandValue highCardValue = eval_hand_value(highCard);
    EXPECT_EQ(highCardValue.getHandType(), HandType::HIGH_CARD);
    HandValue expectedHighCardValue = HandValue::build(
        HandType::HIGH_CARD, {Rank::QUEEN, Rank::TEN, Rank::NINE, Rank::EIGHT, Rank::THREE});
    EXPECT_EQ(highCardValue, expectedHighCardValue);

    cards_5_t straight = {Card("2c"), Card("Ah"), Card("4h"), Card("3d"),
                          Card("5s")}; // 顺子 2, A, 4, 3, 5 -> 5, 4, 3, 2, A
    HandValue straightValue = eval_hand_value(straight);
    EXPECT_EQ(straightValue.getHandType(), HandType::STRAIGHT);
    EXPECT_EQ(straightValue.getRank(0), Rank::FIVE);
    EXPECT_EQ(straightValue.getRank(1), Rank::FOUR);
    EXPECT_EQ(straightValue.getRank(2), Rank::THREE);
    EXPECT_EQ(straightValue.getRank(3), Rank::TWO);
    EXPECT_EQ(straightValue.getRank(4), Rank::ACE);

    cards_5_t twoPair = {
        Card("2c"), Card("2d"), Card("Ts"), Card("Qs"), Card("Tc"),
    }; // 两对 2, 2, 10, Q, 10 -> 10, 10, 2, 2, Q
    HandValue twoPairValue = eval_hand_value(twoPair);
    EXPECT_EQ(twoPairValue.getHandType(), HandType::TWO_PAIR);
    EXPECT_EQ(twoPairValue.getRank(0), Rank::TEN);
    EXPECT_EQ(twoPairValue.getRank(1), Rank::TWO);
    EXPECT_EQ(twoPairValue.getRank(2), Rank::QUEEN);
}

// 测试最终评估结果
TEST(EvalHandTest, FinalEvaluation) {
    cards_7_t seven_card = {Card("Kc"), Card("Qc"), Card("Qs"), Card("Qd"),
                            Card("Kd"), Card("Ac"), Card("Jc")};
    cards_5_t best_five = {Card("Qc"), Card("Qs"), Card("Qd"), Card("Kc"), Card("Kd")};
    HandValue best_five_value = eval_hand_value(best_five);
    EXPECT_EQ(eval_hand(seven_card), best_five_value);
}