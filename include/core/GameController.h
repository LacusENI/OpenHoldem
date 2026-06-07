#pragma once

#include <memory>

namespace poker {
class GameModel;

/**
 * @brief 游戏控制器
 */
class GameController {
public:
    std::unique_ptr<GameModel> model;

    explicit GameController(
        std::unique_ptr<GameModel> model);
    ~GameController();

    void run();
};

} // namespace poker
