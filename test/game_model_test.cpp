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

    /* 初始化翻前：设置筹码 -> 建底 -> 下盲注 -> 排行动顺序 */
    void startPreflop(const std::vector<chips_t>& stacks, seat_t btn) {
        player_set.resize(static_cast<int>(stacks.size()));
        for (int s = 0; s < player_set.playerCount(); ++s) {
            player_set.playerAt(s).setChips(stacks[s]);
        }
        seat_manager.setBtnSeat(btn);
        model.setCurrentStage(Stage::PREFLOP);
        model.startBettingRound();
        EXPECT_NO_THROW(model.postSmallBlind());
        EXPECT_NO_THROW(model.postBigBlind());
        model.startActionRound();
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
    EXPECT_EQ(2, rig.action_flow.getPendingPlayerCount());
    EXPECT_EQ(rig.player_set.getActivePlayerCount(), rig.action_flow.getPendingPlayerCount());
}

// 短筹码大盲：投入全部筹码并 all-in，名义大盲仍为 bring-in
TEST(GameModelTest, ShortBigBlindGoesAllIn) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 7}, 0);

    EXPECT_EQ(7, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(0, rig.chips_manager.getPlayerChipsAt(2));
    EXPECT_TRUE(rig.player_set.playerAt(2).isAllIn());
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(2, rig.action_flow.getPendingPlayerCount());
    EXPECT_EQ(rig.player_set.getActivePlayerCount(), rig.action_flow.getPendingPlayerCount());
}

// 0 筹码玩家投盲不应抛异常（P0-1 崩溃点回归）
TEST(GameModelTest, ZeroChipBlindDoesNotThrow) {
    GameRig rig;
    rig.startPreflop({1000, 0, 1000}, 0);

    EXPECT_TRUE(rig.player_set.playerAt(1).isAllIn());
    EXPECT_EQ(0, rig.betting_manager.getPlayerBetAt(1));
}

// 正常筹码：盲注金额与待行动人数保持原有行为
TEST(GameModelTest, NormalBlinds) {
    GameRig rig;
    rig.startPreflop({1000, 1000, 1000}, 0);

    EXPECT_EQ(5, rig.betting_manager.getPlayerBetAt(1));
    EXPECT_EQ(10, rig.betting_manager.getPlayerBetAt(2));
    EXPECT_EQ(10, rig.betting_manager.getRoundBet());
    EXPECT_EQ(3, rig.action_flow.getPendingPlayerCount());
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
