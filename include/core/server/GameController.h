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
    void handle(const Event::PlayerActionSubmitted& event);
    /* 开始一局游戏 */
    void handle(const Event::StartGame& event);
    /* 进入指定阶段 */
    void enterStage(Stage stage);
    /* 开始下注轮 */
    void startBettingRound();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 完成下注轮 */
    void handleRoundEnded();
    /* 执行摊牌阶段 */
    void processShowdown();
    /* 执行结算阶段 */
    void processSettlement();
};

} // namespace poker
