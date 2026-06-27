#pragma once

#include <memory>

#include "Event.h"

namespace poker {
class EventBus;
class GameModel;

/**
 * @brief 游戏控制器
 */
class GameController {
public:
    std::shared_ptr<EventBus> event_bus;
    std::unique_ptr<GameModel> game_model;

    explicit GameController(
        std::shared_ptr<EventBus> event_bus,
        std::unique_ptr<GameModel> model);
    ~GameController();

    /* 订阅事件 */
    void subscribeEvents();

    /* 开始一局游戏 */
    void startGame();
    /* 进入指定阶段 */
    void enterStage(Stage stage);
    /* 开始下注轮 */
    void startBettingRound();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 询问玩家行动 */
    void onAwaitPlayerInput(const Event::AwaitPlayerInput& event);
    /* 处理玩家输入 */
    void handlePlayerInput(const Event::PlayerInputSubmitted& event);
    /* 完成下注轮 */
    void handleRoundEnded();
    /* 执行摊牌阶段 */
    void processShowdown();
    /* 执行结算阶段 */
    void processSettlement();
};

} // namespace poker
