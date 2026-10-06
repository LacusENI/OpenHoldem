#include <gtest/gtest.h>

#include <optional>
#include <vector>

#include "ActionFlow.h"
#include "BettingManager.h"
#include "CardsManager.h"
#include "ChipsManager.h"
#include "EventBus.hpp"
#include "GameController.h"
#include "GameModel.h"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

using namespace poker;

namespace {
struct GameRig {
    EventBus event_bus;
    PlayerSet player_set;
    SeatManager seat_manager{&player_set};
    BettingManager betting_manager{&player_set};
    ActionFlow action_flow{&player_set, &seat_manager};
    CardsManager cards_manager{nullptr, &player_set}; // 本测试不发牌
    ChipsManager chips_manager{&player_set, &seat_manager};
    GameModel model{&event_bus, &action_flow, &betting_manager, &cards_manager, &chips_manager,
                    &player_set, &seat_manager};

    /* 落座并设置筹码、按钮 */
    void seatPlayers(const std::vector<chips_t>& stacks, seat_t btn) {
        player_set.resize(static_cast<int>(stacks.size()));
        for (int s = 0; s < player_set.playerCount(); ++s) {
            player_set.playerAt(s).setChips(stacks[s]);
        }
        seat_manager.setBtnSeat(btn);
    }

    /* 当前仍需行动的玩家数量 */
    int needsToActCount() const {
        return action_flow.needsToActCount(betting_manager.getRoundBet());
    }

    /* 执行一手牌的翻前流程：建底 -> 下盲注 -> 排行动顺序 */
    void beginHand() {
        model.setCurrentStage(Stage::PREFLOP);
        model.startBettingRound();
        EXPECT_NO_THROW(model.postSmallBlind());
        EXPECT_NO_THROW(model.postBigBlind());
        model.startActionRound();
    }

    /* 会话开始并打第一手牌 */
    void startPreflop(const std::vector<chips_t>& stacks, seat_t btn) {
        seatPlayers(stacks, btn);
        model.startGame();
        beginHand();
    }
};
} // namespace

// 短筹码小盲：投入全部筹码并 all-in，且不计入本轮待行动人数（P0-1 回归）
TEST(GameModelTest, ShortSmallBlindGoesAllIn) {
    GameRig rig;
    rig.startPreflop({1000, 3, 1000}, 0); // btn=0, SB=1, BB=2

    EXPECT_EQ(3, rig.betting_manager.getPlayerBetAt(1));
    EXPECT_EQ(0, rig.chips_manager.getPlayerChipsAt(1));
    EXPECT_TRUE(rig.player_set.playerAt(1).isAllIn());
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(2, rig.needsToActCount());
    EXPECT_EQ(rig.player_set.getActivePlayerCount(), rig.needsToActCount());
}

// 短筹码大盲：投入全部筹码并 all-in，名义大盲仍为 bring-in
TEST(GameModelTest, ShortBigBlindGoesAllIn) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 7}, 0);

    EXPECT_EQ(7, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(0, rig.chips_manager.getPlayerChipsAt(2));
    EXPECT_TRUE(rig.player_set.playerAt(2).isAllIn());
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(2, rig.needsToActCount());
    EXPECT_EQ(rig.player_set.getActivePlayerCount(), rig.needsToActCount());
}

// 0 筹码玩家在开局即为 OUT，不参与盲注且不抛异常（P0-4 语义）
TEST(GameModelTest, ZeroChipPlayerIsOutAtHandStart) {
    GameRig rig;
    rig.startPreflop({1000, 0, 1000}, 0);

    EXPECT_TRUE(rig.player_set.playerAt(1).isOut());
    EXPECT_EQ(0, rig.betting_manager.getPlayerBetAt(1));
}

// 正常筹码：盲注金额与待行动人数保持原有行为
TEST(GameModelTest, NormalBlinds) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);

    EXPECT_EQ(5, rig.betting_manager.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(3, rig.needsToActCount());
}

// GameStarted 必须携带真实会话信息，而不是全零（P0-2 回归）
TEST(GameModelTest, StartGamePublishesSessionInfo) {
    GameRig rig;
    rig.player_set.resize(3);
    rig.seat_manager.setBtnSeat(1);
    rig.betting_manager.setBigBlind(20);

    std::optional<Event::GameStarted> captured;
    rig.event_bus.subscribe<Event::GameStarted>(
        [&captured](const Event::GameStarted& event) { captured = event; });

    rig.model.startGame();

    ASSERT_TRUE(captured.has_value());
    EXPECT_EQ(1, captured->btn_seat);
    EXPECT_EQ(20, captured->big_blind);
    EXPECT_EQ(3, captured->player_count);
}

// 无效行动应被拒绝，且不改变任何状态、不切换行动者（P0-3 回归）
TEST(GameModelTest, InvalidActionIsRejected) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);
    const seat_t acting = rig.action_flow.getActingPlayer();

    // 低于跟注额且未 all-in，判定为 VOID
    bool accepted = rig.model.handlePlayerAction(
        Event::PlayerAction{.target_bet = 1, .is_fold = false, .is_all_in = false});

    EXPECT_FALSE(accepted);
    EXPECT_EQ(acting, rig.action_flow.getActingPlayer());
    EXPECT_EQ(0, rig.betting_manager.getPlayerBetAt(acting));
    EXPECT_EQ(1000, rig.chips_manager.getPlayerChipsAt(acting));
}

// 合法跟注应被接受（P0-3 回归）
TEST(GameModelTest, ValidActionIsAccepted) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);
    const seat_t acting = rig.action_flow.getActingPlayer();

    bool accepted = rig.model.handlePlayerAction(
        Event::PlayerAction{.target_bet = 10, .is_fold = false, .is_all_in = false});

    EXPECT_TRUE(accepted);
    EXPECT_EQ(10, rig.betting_manager.getPlayerBetAt(acting));
}

// 控制器收到无效行动后应重新请求同一玩家，而不是切换行动者（P0-3 回归）
TEST(GameControllerTest, RejectsInvalidActionAndReprompts) {
    GameRig rig;
    GameController controller(&rig.event_bus, &rig.model);
    rig.startPreflop({1000, 1000, 1000}, 0);
    const seat_t acting = rig.action_flow.getActingPlayer();

    std::vector<seat_t> prompted;
    rig.event_bus.subscribe<Event::WaitingForAction>(
        [&prompted](const Event::WaitingForAction& event) { prompted.push_back(event.player); });

    controller.handle(Event::PlayerAction{.target_bet = 1, .is_fold = false, .is_all_in = false});

    ASSERT_EQ(1u, prompted.size());
    EXPECT_EQ(acting, prompted.front());
    EXPECT_EQ(acting, rig.action_flow.getActingPlayer());
}

// 开始下一手：单局数据清空、按钮轮转（P0-4 回归）
TEST(GameModelTest, StartNextHandResetsRoundStateAndAdvancesButton) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);
    const seat_t btn_before = rig.seat_manager.getBtnSeat();

    // 制造一些单手牌状态
    rig.model.handlePlayerAction(
        Event::PlayerAction{.target_bet = 10, .is_fold = false, .is_all_in = false});
    rig.cards_manager.setCommunityCards(
        {Card("Ac"), Card("Kd"), Card("Qh"), Card("Js"), Card("2c")});
    rig.model.splitPot();
    ASSERT_FALSE(rig.chips_manager.getPots().empty());

    rig.model.startNextHand();

    // 按钮前进一位
    EXPECT_EQ((btn_before + 1) % 3, rig.seat_manager.getBtnSeat());
    // 底池与公共牌清空
    EXPECT_TRUE(rig.chips_manager.getPots().empty());
    for (const Card& card : rig.cards_manager.getCommunityCards()) {
        EXPECT_EQ(Card(), card);
    }
    // 单局数据复位
    for (const Player& player : rig.player_set) {
        EXPECT_EQ(0, player.getBet());
        EXPECT_EQ(0, player.getCommitted());
        EXPECT_FALSE(player.isActed());
    }
}

// 淘汰（0 筹码）玩家在下一手出局，盲注跳过该座位（P0-4 回归）
TEST(GameModelTest, BustedPlayerBecomesOutAndBlindsSkipHim) {
    GameRig rig;
    rig.startPreflop({0, 1000, 1000, 1000}, 0); // seat0 无筹码
    rig.model.startNextHand();

    EXPECT_TRUE(rig.player_set.playerAt(0).isOut());
    EXPECT_EQ(1, rig.seat_manager.getBtnSeat()); // 按钮从 0 前进到 1

    rig.beginHand();
    // 4 人桌、seat0 已出局、按钮在 1：SB=2, BB=3, UTG=1
    EXPECT_EQ(2, rig.seat_manager.getSbSeat());
    EXPECT_EQ(3, rig.seat_manager.getBbSeat());
    EXPECT_EQ(1, rig.seat_manager.getUtgSeat());
    EXPECT_EQ(3, rig.needsToActCount());
}

// 第二手牌可以正常打完整轮下注（P0-4 回归）
TEST(GameModelTest, SecondHandCanBePlayed) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);
    rig.model.startNextHand();
    rig.beginHand();

    const Event::PlayerAction call = {.target_bet = 10, .is_fold = false, .is_all_in = false};
    rig.model.handlePlayerAction(call);
    while (!rig.model.isBettingRoundEnded()) {
        rig.model.moveActingPlayer();
        rig.model.handlePlayerAction(call);
    }
    EXPECT_TRUE(rig.model.isBettingRoundEnded());

    rig.model.splitPot();
    ASSERT_EQ(1u, rig.chips_manager.getPots().size());
    EXPECT_EQ(30, rig.chips_manager.getPots()[0].chips);
}

// 单挑：按钮=小盲投 5、另一人投 10；翻前按钮先行动、翻后大盲先行动（P1-1 回归）
TEST(GameModelTest, HeadsUpBlindsAndActionOrder) {
    GameRig rig;
    rig.startPreflop({1000, 1000}, 0); // btn=0

    EXPECT_EQ(5, rig.betting_manager.getPlayerBetAt(0));  // 按钮 = 小盲
    EXPECT_EQ(10, rig.betting_manager.getPlayerBetAt(1)); // 非按钮 = 大盲
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(2, rig.needsToActCount());
    EXPECT_EQ(0, rig.action_flow.getActingPlayer()); // 翻前按钮/SB 先行动

    rig.action_flow.setupToStart(Stage::FLOP);
    EXPECT_EQ(1, rig.action_flow.getActingPlayer()); // 翻后大盲先行动
}

// 在局玩家不足 2 人时不应开局，也不广播 GameStarted（P1-2 回归）
TEST(GameModelTest, StartGameRejectedWhenFewerThanTwoPlayers) {
    GameRig rig;
    rig.seatPlayers({1000, 0}, 0); // 仅 seat0 有筹码

    bool started = false;
    rig.event_bus.subscribe<Event::GameStarted>([&started](const Event::GameStarted&) {
        started = true;
    });

    EXPECT_FALSE(rig.model.startGame());
    EXPECT_FALSE(rig.model.canStartHand());
    EXPECT_FALSE(started);
}

// 只剩一名有筹码玩家时，开始下一手应被拒绝（P1-2 回归）
TEST(GameModelTest, StartNextHandRejectedWhenOnlyOnePlayerLeft) {
    GameRig rig;
    rig.seatPlayers({1000, 0}, 0);

    EXPECT_FALSE(rig.model.startNextHand());
}

// 全部弃牌时结算不应崩溃（防御，P1-2 回归）
TEST(GameModelTest, DistributePotWithNoContendersDoesNotCrash) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);
    for (Player& player : rig.player_set) {
        player.fold();
    }

    rig.model.splitPot();
    EXPECT_NO_THROW(rig.model.distributePots());
}

// 无有效行动者时请求行动不应访问非法座位（P2-1 回归）
TEST(GameModelTest, AwaitPlayerActionWithoutActingPlayerDoesNotCrash) {
    GameRig rig;
    rig.seatPlayers({1000, 1000, 1000}, 0);

    EXPECT_EQ(INVALID_SEAT, rig.action_flow.getActingPlayer());
    EXPECT_NO_THROW(rig.model.awaitPlayerAction());
}

// 无人竞争（只剩一人）时整池直接判给唯一存活者，不切分底池
TEST(GameControllerTest, UncontestedWinAwardsWholePotWithoutSplitting) {
    GameRig rig;
    GameController controller(&rig.event_bus, &rig.model);
    rig.startPreflop({1000, 1000, 1000}, 0); // SB=1(5), BB=2(10), UTG=0

    int winners_declared = 0;
    rig.event_bus.subscribe<Event::WinnerDeclared>(
        [&winners_declared](const Event::WinnerDeclared&) { ++winners_declared; });

    const Event::PlayerAction fold = {.target_bet = 0, .is_fold = true, .is_all_in = false};
    controller.handle(fold); // UTG 弃牌
    controller.handle(fold); // SB 弃牌，只剩 BB

    ASSERT_TRUE(rig.model.isOnlyOnePlayerAlive());
    // 整池 = 0 + 5 + 10 = 15，全部判给 BB（起始 1000 - 10 + 15 = 1005）
    EXPECT_EQ(1005, rig.chips_manager.getPlayerChipsAt(2));
    EXPECT_EQ(995, rig.chips_manager.getPlayerChipsAt(1));
    EXPECT_EQ(1000, rig.chips_manager.getPlayerChipsAt(0));
    EXPECT_TRUE(rig.chips_manager.getPots().empty());
    EXPECT_EQ(1, winners_declared); // 不再有"单人池"产生的多余赢池事件
}

// 唯一最高注者未被跟注的部分在轮末退还给本人
TEST(GameModelTest, ReturnUncalledBetRefundsLeader) {
    GameRig rig;
    rig.seatPlayers({1000, 1000, 1000}, 0);

    // 模拟：seat0 实投 200，seat1 实投 100 并 all-in，seat2 弃牌
    rig.chips_manager.moveChipsToPot(0, 200);
    rig.betting_manager.setPlayerBetAt(0, 200);
    rig.chips_manager.moveChipsToPot(1, 100);
    rig.betting_manager.setPlayerBetAt(1, 100);
    rig.player_set.playerAt(1).allIn();
    rig.player_set.playerAt(2).fold();

    std::optional<Event::UncalledBetReturned> captured;
    rig.event_bus.subscribe<Event::UncalledBetReturned>(
        [&captured](const Event::UncalledBetReturned& event) { captured = event; });

    rig.model.returnUncalledBet();

    ASSERT_TRUE(captured.has_value());
    EXPECT_EQ(0, captured->player);
    EXPECT_EQ(100, captured->amount);
    // seat0 拿回 100：chips 800 -> 900、committed 200 -> 100、bet 200 -> 100
    EXPECT_EQ(900, rig.chips_manager.getPlayerChipsAt(0));
    EXPECT_EQ(100, rig.player_set.playerAt(0).getCommitted());
    EXPECT_EQ(100, rig.betting_manager.getPlayerBetAt(0));
}

// 结束下注轮：先退还多余筹码，再重置下注状态
TEST(GameControllerTest, FinishBettingRoundReturnsUncalledBet) {
    GameRig rig;
    GameController controller(&rig.event_bus, &rig.model);
    rig.seatPlayers({1000, 1000, 1000}, 0);

    rig.chips_manager.moveChipsToPot(0, 200);
    rig.betting_manager.setPlayerBetAt(0, 200);
    rig.chips_manager.moveChipsToPot(1, 100);
    rig.betting_manager.setPlayerBetAt(1, 100);
    rig.player_set.playerAt(1).allIn();
    rig.player_set.playerAt(2).fold();

    controller.finishBettingRound();

    EXPECT_EQ(900, rig.chips_manager.getPlayerChipsAt(0)); // 退还 100
    EXPECT_EQ(0, rig.betting_manager.getRoundBet());        // 下注轮已重置
}
