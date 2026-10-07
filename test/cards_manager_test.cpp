#include <gtest/gtest.h>

#include <vector>

#include "Card.hpp"
#include "CardsManager.h"
#include "GameTypes.hpp"
#include "MockDeck.hpp"
#include "PlayerSet.hpp"
#include "gmock/gmock.h"

using namespace poker;
using namespace testing;

TEST(CardsManagerTest, DealCommunityCards) {
    MockDeck mock_deck;
    PlayerSet player_set;
    CardsManager cards_manager(mock_deck, player_set);
    cards_5_t community_cards = {Card(Rank::TWO, Suit::CLUB), Card(Rank::EIGHT, Suit::DIAMOND),
                                 Card(Rank::ACE, Suit::HEART), Card(Rank::JACK, Suit::SPADE),
                                 Card(Rank::QUEEN, Suit::DIAMOND)};
    EXPECT_CALL(mock_deck, deal())
        .WillOnce(Return(community_cards[0]))
        .WillOnce(Return(community_cards[1]))
        .WillOnce(Return(community_cards[2]))
        .WillOnce(Return(community_cards[3]))
        .WillOnce(Return(community_cards[4]));
    cards_5_t expected = {};
    cards_manager.dealCommunityCards(Stage::PREFLOP);
    EXPECT_EQ(expected, cards_manager.getCommunityCards());
    cards_manager.dealCommunityCards(Stage::FLOP);
    expected = {community_cards[0], community_cards[1], community_cards[2], Card(), Card()};
    EXPECT_EQ(expected, cards_manager.getCommunityCards());
    cards_manager.dealCommunityCards(Stage::TURN);
    expected[3] = community_cards[3];
    cards_manager.dealCommunityCards(Stage::RIVER);
    expected[4] = community_cards[4];
    EXPECT_EQ(expected, cards_manager.getCommunityCards());
}

TEST(CardsManagerTest, DealHoleCards) {
    MockDeck mock_deck;
    PlayerSet player_set;
    CardsManager cards_manager(mock_deck, player_set);
    player_set.resize(2);
    cards_2_t hole_cards_1 = {Card(Rank::TWO, Suit::CLUB), Card(Rank::EIGHT, Suit::DIAMOND)};
    cards_2_t hole_cards_2 = {Card(Rank::ACE, Suit::HEART), Card(Rank::JACK, Suit::SPADE)};
    EXPECT_CALL(mock_deck, deal())
        .WillOnce(Return(hole_cards_1[0]))
        .WillOnce(Return(hole_cards_1[1]))
        .WillOnce(Return(hole_cards_2[0]))
        .WillOnce(Return(hole_cards_2[1]));
    cards_manager.dealHoleCards();
    EXPECT_EQ(hole_cards_1, cards_manager.getHoleCardsAt(0));
    EXPECT_EQ(hole_cards_2, cards_manager.getHoleCardsAt(1));
}

TEST(CardsManagerTest, DetermineWinners) {
    PlayerSet player_set;
    MockEmptyDeck deck;
    CardsManager cards_manager(deck, player_set);
    player_set.resize(4);
    cards_5_t community_cards = {Card("Ah"), Card("As"), Card("Kd"), Card("Qc"), Card("Jh")};
    cards_manager.setCommunityCards(community_cards);
    cards_manager.setHoleCardsAt(0, {Card("Kh"), Card("Qs")});
    cards_manager.setHoleCardsAt(1, {Card("Kc"), Card("Js")});
    cards_manager.setHoleCardsAt(2, {Card("Jd"), Card("8c")});
    cards_manager.setHoleCardsAt(3, {Card("Ks"), Card("2h")});
    // 公共牌: ♥A ♠A ♦K ♣Q ♥J
    // 底牌 [0]: ♥K ♠Q [1]: ♣K ♠J [2] ♦J ♣8 [3] ♠K ♥2
    // 手牌 [0]: AK两对 踢脚牌Q [1]: AK两对 踢脚牌Q [2]: 三条A 踢脚牌K, Q [3]: AK两对 踢脚牌Q
    auto view = player_set.getAlivePlayers();
    std::vector<seat_t> players(view.begin(), view.end());
    std::vector<seat_t> winners = cards_manager.determineWinners(players);
    std::vector<seat_t> expected = {0, 1, 3};
    EXPECT_EQ(expected, winners);
}