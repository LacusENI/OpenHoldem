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
    SeatManager* seat_manager;
    Stage current_stage = Stage::IDLE;

    /* 广播当前会话信息（按钮位、大盲、人数） */
    void publishGameStarted();

public:
    GameModel(EventBus* event_bus, ActionFlow* action_flow, BettingManager* betting_manager,
              CardsManager* cards_manager, ChipsManager* chips_manager, PlayerSet* player_set,
              SeatManager* seat_manager)
        : event_bus(event_bus),
          action_flow(action_flow),
          betting_manager(betting_manager),
          cards_manager(cards_manager),
          chips_manager(chips_manager),
          player_set(player_set),
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
    SeatManager* getSeatManager() const { return seat_manager; }

    /* 改变游戏阶段 */
    void changeStage(Stage new_stage);
    /* 移至下一个阶段 */
    void advanceStage() { changeStage(next_stage(current_stage)); }

    /* 加入新玩家 */
    void joinNewPlayer();

    /* 开始一局游戏（会话开始，初始化第一手牌状态）；玩家不足 2 人返回 false */
    bool startGame();
    /* 开始下一手牌：重置牌局状态并轮转按钮；在局玩家不足 2 人返回 false */
    bool startNextHand();
    /* 重置为一手牌开始前的状态（不轮转按钮、不发布事件） */
    void resetForNewHand();
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
    /* 初始化本轮行动顺序（应在盲注投入之后调用） */
    void startActionRound();
    /* 关闭下注轮 */
    void closeBettingRound();
    /* 退还被唯一最高下注者未被跟注的多余筹码 */
    void returnUncalledBet();
    /* 等待玩家行动 */
    void awaitPlayerAction();
    /* 处理玩家行动；返回该行动是否被接受（无效输入返回 false） */
    bool handlePlayerAction(const Event::PlayerAction& event);
    /* 移至下一个行动的玩家 */
    void moveActingPlayer();

    /* 揭露所有玩家的底牌 */
    void revealAllHoleCards();
    /* 计算所有的底池 */
    void splitPot();
    /* 无人竞争时结算：整池判给唯一存活玩家 */
    void settleUncontested();
    /* 瓜分所有的底池 */
    void distributePots();

    /* 下注轮是否已结束 */
    bool isBettingRoundEnded() const;
    /* 是否只有一名玩家留有手牌 */
    bool isOnlyOnePlayerAlive() const;
    /* 所有下注轮是否已经结束 */
    bool isStreetsFinished() const;
    /* 没有任何玩家留在牌局 */
    bool isNoOnePlayerAlive() const;
    /* 是否满足开始一手牌的条件（至少两名在局玩家） */
    bool canStartHand() const;
};
} // namespace poker
