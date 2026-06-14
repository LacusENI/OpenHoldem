#pragma once

#include <memory>

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

    /* 开始一局游戏 */
    void startGame();
    /* 开始一个阶段 */
    void startStreet();
    /* 开始下注轮 */
    void startBettingRound();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 完成下注轮 */
    void finishBettingRound();
    /* 移至下一个阶段 */
    void advanceToNextStreet();
    /* 进入摊牌阶段 */
    void enterShowdownStreet();
};

} // namespace poker
