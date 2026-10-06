#pragma once


#include "Event.hpp"

namespace poker {
class EventBus;
class GameModel;

/**
 * @brief 游戏控制器
 */
class GameController {
public:
    EventBus* event_bus;
    GameModel* game_model;

    explicit GameController(EventBus* event_bus, GameModel* model);
    ~GameController() = default;

    /* 订阅事件 */
    void subscribeEvent();
    /* 处理玩家加入请求 */
    void handle(const Event::PlayerJoinRequest& event);
    /* 处理玩家输入 */
    void handle(const Event::PlayerAction& event);
    /* 开始一局游戏 */
    void handle(const Event::StartGame& event);

    /* 开始下一手牌（重置状态并轮转按钮） */
    void startNextHand();

    /* 结束本下注轮：退还未跟注筹码、重置下注状态并发布结束事件 */
    void finishBettingRound();
    /* 无人竞争：整池判给唯一存活玩家 */
    void handleUncontestedPot();
    /* 已无可继续下注者：直接进入摊牌 */
    void handleStreetsFinished();
    /* 进入下一阶段；河牌则进入摊牌 */
    void advanceToNextStage();

    /* 翻牌前阶段 */
    void processPreflopStage();
    /* 翻牌后阶段 */
    void processPostflopStage();
    /* 摊牌阶段 */
    void processShowdownStage(); 
};

} // namespace poker
