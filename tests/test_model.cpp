#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "GameModel.h"

using namespace holdem;

class MockDeck : public IDeck {
public:
    MOCK_METHOD(void, shuffle, (), (override));
    MOCK_METHOD(Card, deal, (), (override));
};

class MockEmptyDeck : public IDeck {
public:
    void shuffle() override {}

    Card deal() override { return Card(); }
};

using testing::Return;

// 测试是否正确发放底牌和公共牌
TEST(TestGameModel, TestDealCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEART, Rank::ACE)))     // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::ACE)))     // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEART, Rank::KING)))    // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::KING)))    // 玩家2 第二张
        .WillOnce(Return(Card(Suit::DIAMOND, Rank::JACK)))  // 第一张公共牌
        .WillOnce(Return(Card(Suit::DIAMOND, Rank::QUEEN))) // 第二张公共牌
        .WillOnce(Return(Card(Suit::HEART, Rank::EIGHT)))   // 第三张公共牌
        .WillOnce(Return(Card(Suit::CLUB, Rank::THREE)))    // 第四张公共牌
        .WillOnce(Return(Card(Suit::DIAMOND, Rank::TEN)));  // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    GameModel model(std::move(deck));
    model.addPlayer(1);
    model.addPlayer(2);

    // Pre-flop 阶段
    model.game_state = GameState::PREFLOP;
    model.dealCards();
    // 验证底牌是否发放正确
    Player& player1 = model.getPlayer(0);
    Player& player2 = model.getPlayer(1);
    EXPECT_EQ(player1.hole_cards[0], Card("HA"));
    EXPECT_EQ(player1.hole_cards[1], Card("SA"));
    EXPECT_EQ(player2.hole_cards[0], Card("HK"));
    EXPECT_EQ(player2.hole_cards[1], Card("SK"));

    // 验证公共牌的发放
    Cards5& communityCards = model.community_cards;
    // Flop 阶段
    model.game_state = GameState::FLOP;
    model.dealCards();
    EXPECT_EQ(communityCards[0], Card("DJ"));
    EXPECT_EQ(communityCards[1], Card("DQ"));
    EXPECT_EQ(communityCards[2], Card("H8"));

    // Turn 阶段
    model.game_state = GameState::TURN;
    model.dealCards();
    EXPECT_EQ(communityCards[3], Card("C3"));

    // River 阶段
    model.game_state = GameState::RIVER;
    model.dealCards();
    EXPECT_EQ(communityCards[4], Card("DT"));
}

TEST(TestGameModel, TestCommitChips) {
    auto deck = std::make_unique<MockEmptyDeck>();
    GameModel model(std::move(deck));
    model.addPlayer(1);
    Player& player = model.getPlayer(0);

    // 测试 Bet 动作
    player.chips = 20;
    player.current_bet = 0;
    model.pot = 0;
    model.round_bet = 0;
    model.commitChips(0, 10);

    EXPECT_EQ(player.current_bet, 10);
    EXPECT_EQ(player.chips, 10);
    EXPECT_EQ(model.pot, 10);
    EXPECT_EQ(model.round_bet, 10);

    // 测试 Check 动作
    player.chips = 20;
    player.current_bet = 0;
    model.pot = 0;
    model.round_bet = 0;
    model.commitChips(0, 0);

    EXPECT_EQ(player.current_bet, 0);
    EXPECT_EQ(player.chips, 20);
    EXPECT_EQ(model.pot, 0);
    EXPECT_EQ(model.round_bet, 0);

    // 测试 Raise 动作
    player.chips = 20;
    player.current_bet = 10;
    model.pot = 50;
    model.round_bet = 10;
    model.commitChips(0, 10);

    EXPECT_EQ(player.chips, 10);
    EXPECT_EQ(player.current_bet, 20);
    EXPECT_EQ(model.pot, 60);
    EXPECT_EQ(model.round_bet, 20);
}