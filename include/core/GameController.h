#pragma once

#include <memory>

namespace poker {
class GameModel;

/**
 * @brief 负责一局游戏
 */
class GameController {
public:
    std::unique_ptr<GameModel> model;

    explicit GameController(
        std::unique_ptr<GameModel> model);
    ~GameController();

    void run();
};

} // namespace holdem
