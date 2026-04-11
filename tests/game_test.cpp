#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Game.h"

using namespace holdem;

class MockDeck : public IDeck {
public:
    MOCK_METHOD(void, shuffle, (), (override));
    MOCK_METHOD(Card, deal, (), (override));
};

using testing::Return;

// 测试是否正确发放底牌
TEST(TestGame, DealHoleCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card("HA")))  // 玩家1 第一张
        .WillOnce(Return(Card("SA")))  // 玩家1 第二张
        .WillOnce(Return(Card("HK")))  // 玩家2 第一张
        .WillOnce(Return(Card("SK"))); // 玩家2 第二张
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);

    game.nextStreet();
    Player& player1 = game.getPlayer(0);
    Player& player2 = game.getPlayer(1);

    EXPECT_EQ(player1.hole_cards[0], Card("HA"));
    EXPECT_EQ(player1.hole_cards[1], Card("SA"));
    EXPECT_EQ(player2.hole_cards[0], Card("HK"));
    EXPECT_EQ(player2.hole_cards[1], Card("SK"));
}

// 测试三个阶段是否正确发放公共牌
TEST(TestGame, DeclCommunityCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card("HA")))  // 玩家1 第一张
        .WillOnce(Return(Card("SA")))  // 玩家1 第二张
        .WillOnce(Return(Card("HK")))  // 玩家2 第一张
        .WillOnce(Return(Card("SK")))  // 玩家2 第二张
        .WillOnce(Return(Card("DJ")))  // 第一张公共牌
        .WillOnce(Return(Card("DQ")))  // 第二张公共牌
        .WillOnce(Return(Card("H8")))  // 第三张公共牌
        .WillOnce(Return(Card("C3")))  // 第四张公共牌
        .WillOnce(Return(Card("DT"))); // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);

    game.nextStreet(); // 进入 Preflop 阶段
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    Cards5& communityCards = game.model.community_cards;

    EXPECT_EQ(communityCards[0], Card("DJ"));
    EXPECT_EQ(communityCards[1], Card("DQ"));
    EXPECT_EQ(communityCards[2], Card("H8"));
    EXPECT_EQ(communityCards[3], Card("C3"));
    EXPECT_EQ(communityCards[4], Card("DT"));
}

// 测试是否得出正确的赢家
TEST(TestGame, GetWinners) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card("HA")))  // 玩家1 第一张
        .WillOnce(Return(Card("SA")))  // 玩家1 第二张
        .WillOnce(Return(Card("HK")))  // 玩家2 第一张
        .WillOnce(Return(Card("SK")))  // 玩家2 第二张
        .WillOnce(Return(Card("DJ")))  // 第一张公共牌
        .WillOnce(Return(Card("DQ")))  // 第二张公共牌
        .WillOnce(Return(Card("H8")))  // 第三张公共牌
        .WillOnce(Return(Card("C3")))  // 第四张公共牌
        .WillOnce(Return(Card("DT"))); // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);
    game.nextStreet(); // 进入 Preflop 阶段
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    game.handleAward(); // 摊牌

    auto& winners = game.model.winners;
    ASSERT_EQ(winners.size(), 1);

    // 玩家1: 一对A, 玩家2: 一对K
    EXPECT_EQ(winners[0], game.getPlayer(0).position);
}

// 测试是否正确分配底池
TEST(TestGame, TestAward) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card("HA")))  // 玩家1 第一张
        .WillOnce(Return(Card("SA")))  // 玩家1 第二张
        .WillOnce(Return(Card("HK")))  // 玩家2 第一张
        .WillOnce(Return(Card("SK")))  // 玩家2 第二张
        .WillOnce(Return(Card("DJ")))  // 第一张公共牌
        .WillOnce(Return(Card("DQ")))  // 第二张公共牌
        .WillOnce(Return(Card("H8")))  // 第三张公共牌
        .WillOnce(Return(Card("C3")))  // 第四张公共牌
        .WillOnce(Return(Card("DT"))); // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);
    game.nextStreet(); // 进入 Preflop 阶段
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    game.handleAward();
    Stack chips_before_award = game.getPlayer(0).chips;
    // 玩家1: 一对A, 玩家2: 一对K
    Player& winner = game.getPlayer(game.model.winners[0]);
    Stack pot = game.model.pot;
    EXPECT_EQ(winner.chips, pot + chips_before_award);
}