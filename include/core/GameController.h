#pragma once

#include <memory>

namespace holdem {
class GameModel;
class IGameView;

/**
 * @brief 负责一局游戏
 */
class GameController {
public:
    std::unique_ptr<GameModel> model;
    std::unique_ptr<IGameView> view;

    explicit GameController(
        std::unique_ptr<GameModel> model,
        std::unique_ptr<IGameView> view);
    ~GameController();

    void run();
};

} // namespace holdem
