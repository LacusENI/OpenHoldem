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

    void handlePlayerJoining(const Event::PlayerJoinRequest& event);
    /* 开始一局游戏 */
    void startGame();
    /* 进入指定阶段 */
    void enterStage(Stage stage);
    /* 开始下注轮 */
    void startBettingRound();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 处理玩家输入 */
    void handlePlayerInput(const Event::PlayerActionSubmitted& event);
    /* 完成下注轮 */
    void handleRoundEnded();
    /* 执行摊牌阶段 */
    void processShowdown();
    /* 执行结算阶段 */
    void processSettlement();
};

} // namespace poker
