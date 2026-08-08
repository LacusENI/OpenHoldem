#pragma once

#include <vector>

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class EventBus;
class ActionFlow;
class BettingManager;
class CardsManager;
class ChipsManager;
class PlayerSet;
class PotManager;
class SeatManager;

/**
 * @brief 游戏领域模型
 */
class GameModel {
private:
    EventBus* event_bus;
    ActionFlow* action_flow;
    BettingManager* betting_manager;
    CardsManager* cards_manager;
    ChipsManager* chips_manager;
    PlayerSet* player_set;
    PotManager* pot_manager;
    SeatManager* seat_manager;
    Stage current_stage = Stage::IDLE;

public:
    GameModel(EventBus* event_bus, ActionFlow* action_flow, BettingManager* betting_manager,
              CardsManager* cards_manager, ChipsManager* chips_manager, PlayerSet* player_set,
              PotManager* pot_manager, SeatManager* seat_manager)
        : event_bus(event_bus),
          action_flow(action_flow),
          betting_manager(betting_manager),
          cards_manager(cards_manager),
          chips_manager(chips_manager),
          player_set(player_set),
          pot_manager(pot_manager),
          seat_manager(seat_manager) {}
    ~GameModel() = default;

    Stage getCurrentStage() const { return current_stage; }
    void setCurrentStage(Stage stage) { current_stage = stage; }

    EventBus* getEventBus() const { return event_bus; }
    ActionFlow* getActionFlow() const { return action_flow; }
    BettingManager* getBettingManager() const { return betting_manager; }
    CardsManager* getCardsManager() const { return cards_manager; }
    ChipsManager* getChipsManager() const { return chips_manager; }
    PlayerSet* getPlayerSet() const { return player_set; }
    PotManager* getPotManager() const { return pot_manager; }
    SeatManager* getSeatManager() const { return seat_manager; }

    /* 改变游戏阶段 */
    void changeStage(Stage new_stage);
    /* 移至下一个阶段 */
    void advanceStage() { changeStage(next_stage(current_stage)); }

    /* 加入新玩家 */
    void joinNewPlayer();

    /* 开始一局游戏 */
    void startGame();
    /* 结束本局游戏 */
    void finishGame();
    /* 洗牌 */
    void shuffleDeck();
    /* 给玩家发放底牌 */
    void dealHoleCards();
    /* 发放公共牌 */
    void dealCommunityCards();
    /* 执行小盲注 */
    void postSmallBlind();
    /* 执行大盲注 */
    void postBigBlind();

    /* 开始下注轮 */
    void startBettingRound();
    /* 关闭下注轮 */
    void closeBettingRound();
    /* 等待玩家行动 */
    void awaitPlayerAction();
    /* 处理玩家行动 */
    void handlePlayerAction(const Event::PlayerAction& event);
    /* 移至下一个行动的玩家 */
    void moveActingPlayer();

    /* 揭露玩家底牌 */
    void revealHoleCards();
    /* 比较玩家手牌大小 */
    std::vector<seat_t> comparePlayerHands();
    /* 分配底池给赢家 */
    void awardPotToWinners(std::vector<seat_t>& winners);

    /* 下注轮是否已结束 */
    bool isBettingRoundEnded() const;
    /* 是否只有一名玩家留有手牌 */
    bool isOnlyOnePlayerAlive() const;
    /* 是否应该直接跳转至摊牌阶段  */
    bool shouldSkipToShowdown() const;
    /* 没有任何玩家留在牌局 */
    bool isNoOnePlayerAlive() const;
};
} // namespace poker
