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
    /* 进入 Pre-flop 阶段 */
    void enterPreflopStreet();

};

} // namespace poker
