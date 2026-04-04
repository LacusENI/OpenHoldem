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
        .WillOnce(Return(Card(Suit::HEART, Rank::ACE)))   // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::ACE)))   // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEART, Rank::KING)))  // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::KING))); // 玩家2 第二张
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);

    game.dealHoleCards();

    auto players = game.getPlayers();
    ASSERT_EQ(players.size(), 2);
    Player& player1 = players[0];
    Player& player2 = players[1];

    EXPECT_EQ(player1.hole_cards[0], Card(Suit::HEART, Rank::ACE));
    EXPECT_EQ(player1.hole_cards[1], Card(Suit::SPADE, Rank::ACE));
    EXPECT_EQ(player2.hole_cards[0], Card(Suit::HEART, Rank::KING));
    EXPECT_EQ(player2.hole_cards[1], Card(Suit::SPADE, Rank::KING));
}

// 测试 Preflop 阶段发的公共牌是否正确
TEST(TestGame, FlopDealCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEART, Rank::ACE)))     // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::ACE)))     // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEART, Rank::KING)))    // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADE, Rank::KING)))    // 玩家2 第二张
        .WillOnce(Return(Card(Suit::DIAMOND, Rank::JACK)))  // 第一张公共牌
        .WillOnce(Return(Card(Suit::DIAMOND, Rank::QUEEN))) // 第二张公共牌
        .WillOnce(Return(Card(Suit::HEART, Rank::EIGHT)));  // 第三张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);

    game.dealHoleCards();
    game.nextStreet(); // 进入 flop 阶段
    auto communityCards = game.getCommunityCards();

    EXPECT_EQ(communityCards[0], Card(Suit::DIAMOND, Rank::JACK));
    EXPECT_EQ(communityCards[1], Card(Suit::DIAMOND, Rank::QUEEN));
    EXPECT_EQ(communityCards[2], Card(Suit::HEART, Rank::EIGHT));
}

// 测试三个阶段是否正确发放公共牌
TEST(TestGame, DeclCommunityCards) {
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

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);

    game.dealHoleCards();
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    auto communityCards = game.getCommunityCards();

    EXPECT_EQ(communityCards[0], Card(Suit::DIAMOND, Rank::JACK));
    EXPECT_EQ(communityCards[1], Card(Suit::DIAMOND, Rank::QUEEN));
    EXPECT_EQ(communityCards[2], Card(Suit::HEART, Rank::EIGHT));
    EXPECT_EQ(communityCards[3], Card(Suit::CLUB, Rank::THREE));
    EXPECT_EQ(communityCards[4], Card(Suit::DIAMOND, Rank::TEN));
}

// 测试是否得出正确的赢家
TEST(TestGame, GetWinners) {
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

    Game game(std::move(deck));
    game.addPlayer(0);
    game.addPlayer(1);
    game.dealHoleCards();
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    game.showdown(); // 摊牌

    auto& winners = game.getWinners();
    auto& players = game.getPlayers();
    ASSERT_EQ(winners.size(), 1);

    // 玩家1: 一对A, 玩家2: 一对K
    EXPECT_EQ(winners[0], players[0].position);
    EXPECT_EQ(winners[0], 0);
    EXPECT_EQ(&game.getPlayerByPosition(winners[0]), &players[0]);
}

// 测试手牌描述信息是否正确
TEST(TestGame, TestHandMessage) {
    const Cards5 hand5 = {
        Card("C7"), Card("D8"), Card("H9"),
        Card("ST"), Card("SJ")
    };
    HandValue hand_value = HandEvaluator::getHandValue(hand5);
    std::string hand_message = internal::getHandMessage(hand_value);

    EXPECT_EQ(hand_value.getHandType(), HandType::STRAIGHT);
    EXPECT_EQ(hand_message, "Straight, 7 to Jack");
}