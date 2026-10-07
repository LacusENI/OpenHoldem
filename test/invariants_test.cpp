#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <vector>

#include "Deck.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameSession.h"
#include "GameTypes.hpp"
#include "Player.hpp"
#include "PlayerSet.hpp"

using namespace poker;

namespace {
/* 用真实（可播种）牌堆跑完整一手，以便覆盖发牌与摊牌 */
struct InvariantRig {
    Deck deck;
    GameSession session{deck};
    EventBus& event_bus = session.event_bus;
    PlayerSet& players = session.player_set;
    SeatManager& seats = session.seat_manager;
    BettingRound& betting = session.betting_round;
    GameModel& model = session.model;
    GameController& controller = session.controller;

    explicit InvariantRig(std::uint32_t seed) : deck(seed) {}

    /* 恒真不变量 */
    void checkInvariants(chips_t total) const {
        chips_t sum = 0;
        chips_t max_bet = 0;
        for (seat_t s = 0; s < players.playerCount(); ++s) {
            const Player& p = players.playerAt(s);
            EXPECT_GE(p.getChips(), 0);
            EXPECT_GE(p.getBet(), 0);
            EXPECT_GE(p.getCommitted(), 0);
            EXPECT_LE(p.getBet(), p.getCommitted());
            if (!p.isOut()) EXPECT_EQ(p.isAllIn(), p.getChips() == 0);
            sum += p.getChips() + p.getCommitted();
            max_bet = std::max(max_bet, p.getBet());
        }
        EXPECT_EQ(total, sum);                     // I1 筹码守恒
        EXPECT_EQ(betting.getRoundBet(), max_bet); // I3 round_bet 为最高注
    }
};

/* 以指定种子打完整一手（自动跟注/过牌），期间与结算后校验不变量 */
void runFullHand(std::uint32_t seed) {
    InvariantRig rig(seed);
    rig.controller.subscribeEvent();

    const chips_t total = 3000;
    bool ended = false;
    std::vector<Stage> stages;
    rig.event_bus.subscribe<Event::StageChanged>(
        [&](const Event::StageChanged& s) { stages.push_back(s.stage); });
    rig.event_bus.subscribe<Event::WaitingForAction>([&](const Event::WaitingForAction& e) {
        rig.checkInvariants(total);
        rig.event_bus.publish(Event::PlayerAction{
            .target_bet = e.round_bet, .is_fold = false, .is_all_in = false});
    });
    rig.event_bus.subscribe<Event::GameEnded>([&](const Event::GameEnded&) {
        rig.checkInvariants(total);
        ended = true;
    });

    rig.event_bus.publish(Event::PlayerJoinRequest{});
    rig.event_bus.publish(Event::PlayerJoinRequest{});
    rig.event_bus.publish(Event::PlayerJoinRequest{});
    for (seat_t s = 0; s < 3; ++s) {
        rig.players.playerAt(s).setChips(1000);
    }
    rig.event_bus.publish(Event::StartGame{});

    EXPECT_TRUE(ended);
    chips_t committed_sum = 0;
    for (seat_t s = 0; s < rig.players.playerCount(); ++s) {
        committed_sum += rig.players.playerAt(s).getCommitted();
    }
    EXPECT_EQ(0, committed_sum); // 结算后 committed 清零

    // 状态机：完整一手应依次经过 PREFLOP→FLOP→TURN→RIVER→SHOWDOWN→SETTLEMENT
    const std::vector<Stage> expected = {Stage::PREFLOP,  Stage::FLOP,      Stage::TURN,
                                         Stage::RIVER,    Stage::SHOWDOWN,  Stage::SETTLEMENT};
    EXPECT_EQ(expected, stages);
}
} // namespace

// 多组固定种子下打完整一手（含摊牌）：筹码守恒、下注/状态自洽、结算后 committed 清零
TEST(InvariantsTest, FullHandConservesChipsAndClearsCommitted) {
    for (std::uint32_t seed : {1u, 2u, 3u, 42u, 123u, 777u, 2024u, 99991u}) {
        runFullHand(seed);
    }
}

// 边池切分守恒：Σ pot.chips == Σ committed
TEST(InvariantsTest, SidePotSumEqualsCommitted) {
    Deck deck(1);
    GameSession session(deck);
    PlayerSet& players = session.player_set;
    players.resize(3);
    for (seat_t s = 0; s < 3; ++s) {
        players.playerAt(s).setChips(1000);
    }
    players.playerAt(0).commit(100); // 下注并支付
    players.playerAt(1).commit(50);
    players.playerAt(2).commit(200);

    session.pot_manager.splitThePot();

    chips_t pot_sum = 0;
    for (const Pot& pot : session.pot_manager.getPots()) {
        pot_sum += pot.chips;
        EXPECT_FALSE(pot.contributors.empty());
    }
    chips_t committed_sum = 0;
    for (seat_t s = 0; s < 3; ++s) {
        committed_sum += players.playerAt(s).getCommitted();
    }
    EXPECT_EQ(committed_sum, pot_sum);
}
