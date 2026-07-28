#include <gtest/gtest.h>

#include <vector>

#include "Card.hpp"
#include "CardsManager.h"
#include "Deck.hpp"
#include "GameTypes.hpp"
#include "PlayerSet.h"

using namespace poker;

TEST(CardsManagerTest, DetermineWinners) {
    Deck deck;
    PlayerSet player_set;
    CardsManager cards_manager(&deck, &player_set);
    player_set.resize(4);
    cards_5_t community_cards = {Card("HA"), Card("SA"), Card("DK"), Card("CQ"), Card("HJ")};
    cards_manager.setCommunityCards(community_cards);
    cards_manager.setHoleCardsAt(0, {Card("HK"), Card("SQ")});
    cards_manager.setHoleCardsAt(1, {Card("CK"), Card("SJ")});
    cards_manager.setHoleCardsAt(2, {Card("DJ"), Card("C8")});
    cards_manager.setHoleCardsAt(3, {Card("SK"), Card("H2")});
    // 公共牌: ♥A ♠A ♦K ♣Q ♥J
    // 底牌 [0]: ♥K ♠Q [1]: ♣K ♠J [2] ♦J ♣8 [3] ♠K ♥2
    // 手牌 [0]: AK两对 踢脚牌Q [1]: AK两对 踢脚牌Q [2]: 三条A 踢脚牌K, Q [3]: AK两对 踢脚牌Q
    std::vector<seat_idx_t> winners = cards_manager.determineWinners();
    std::vector<seat_idx_t> expected = {0, 1, 3};
    EXPECT_EQ(expected, winners);
}