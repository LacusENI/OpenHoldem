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

TEST(TestGame, DealHoleCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEARTS, Rank::ACE)))   // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::ACE)))   // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEARTS, Rank::KING)))  // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::KING))); // 玩家2 第二张
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(Player());
    game.addPlayer(Player());

    game.dealHoleCards();

    auto players = game.getPlayers();
    ASSERT_EQ(players.size(), 2);
    Player& player1 = players[0];
    Player& player2 = players[1];

    EXPECT_EQ(player1.hole_cards[0], Card(Suit::HEARTS, Rank::ACE));
    EXPECT_EQ(player1.hole_cards[1], Card(Suit::SPADES, Rank::ACE));
    EXPECT_EQ(player2.hole_cards[0], Card(Suit::HEARTS, Rank::KING));
    EXPECT_EQ(player2.hole_cards[1], Card(Suit::SPADES, Rank::KING));
}

TEST(TestGame, FlopDealCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEARTS, Rank::ACE)))     // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::ACE)))     // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEARTS, Rank::KING)))    // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::KING)))    // 玩家2 第二张
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::JACK)))  // 第一张公共牌
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::QUEEN))) // 第二张公共牌
        .WillOnce(Return(Card(Suit::HEARTS, Rank::EIGHT)));  // 第三张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(Player());
    game.addPlayer(Player());

    game.dealHoleCards();
    game.nextStreet(); // 进入 flop 阶段
    auto communityCards = game.getCommunityCards();

    EXPECT_EQ(communityCards[0], Card(Suit::DIAMONDS, Rank::JACK));
    EXPECT_EQ(communityCards[1], Card(Suit::DIAMONDS, Rank::QUEEN));
    EXPECT_EQ(communityCards[2], Card(Suit::HEARTS, Rank::EIGHT));
}

TEST(TestGame, DeclCommunityCards) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEARTS, Rank::ACE)))     // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::ACE)))     // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEARTS, Rank::KING)))    // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::KING)))    // 玩家2 第二张
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::JACK)))  // 第一张公共牌
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::QUEEN))) // 第二张公共牌
        .WillOnce(Return(Card(Suit::HEARTS, Rank::EIGHT)))   // 第三张公共牌
        .WillOnce(Return(Card(Suit::CLUBS, Rank::THREE)))    // 第四张公共牌
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::TEN)));  // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(Player());
    game.addPlayer(Player());

    game.dealHoleCards();
    game.nextStreet(); // 进入 flop 阶段
    game.nextStreet(); // 进入 turn 阶段
    game.nextStreet(); // 进入 river 阶段
    auto communityCards = game.getCommunityCards();

    EXPECT_EQ(communityCards[0], Card(Suit::DIAMONDS, Rank::JACK));
    EXPECT_EQ(communityCards[1], Card(Suit::DIAMONDS, Rank::QUEEN));
    EXPECT_EQ(communityCards[2], Card(Suit::HEARTS, Rank::EIGHT));
    EXPECT_EQ(communityCards[3], Card(Suit::CLUBS, Rank::THREE));
    EXPECT_EQ(communityCards[4], Card(Suit::DIAMONDS, Rank::TEN));
}

TEST(TestGame, GetWinners) {
    auto deck = std::make_unique<MockDeck>();
    EXPECT_CALL(*deck, deal())
        .WillOnce(Return(Card(Suit::HEARTS, Rank::ACE)))     // 玩家1 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::ACE)))     // 玩家1 第二张
        .WillOnce(Return(Card(Suit::HEARTS, Rank::KING)))    // 玩家2 第一张
        .WillOnce(Return(Card(Suit::SPADES, Rank::KING)))    // 玩家2 第二张
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::JACK)))  // 第一张公共牌
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::QUEEN))) // 第二张公共牌
        .WillOnce(Return(Card(Suit::HEARTS, Rank::EIGHT)))   // 第三张公共牌
        .WillOnce(Return(Card(Suit::CLUBS, Rank::THREE)))    // 第四张公共牌
        .WillOnce(Return(Card(Suit::DIAMONDS, Rank::TEN)));  // 第五张公共牌
    EXPECT_CALL(*deck, shuffle()).Times(testing::AnyNumber());

    Game game(std::move(deck));
    game.addPlayer(Player());
    game.addPlayer(Player());
    game.run();

    auto& winners = game.getWinners();
    auto& players = game.getPlayers();
    ASSERT_EQ(winners.size(), 1);

    // 玩家1: 一对A, 玩家2: 一对K
    EXPECT_EQ(winners[0], &players[0]);
}