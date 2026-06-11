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

    void startGame();

    void enterPreflopStreet();
};

} // namespace poker
