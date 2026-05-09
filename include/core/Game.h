#pragma once

#include <memory>

namespace holdem {
class GameModel;
class IGameView;

/**
 * @brief 负责一局游戏
 */
class Game {
public:
    std::unique_ptr<GameModel> model;
    std::unique_ptr<IGameView> view;

    explicit Game(
        std::unique_ptr<GameModel> model,
        std::unique_ptr<IGameView> view);
    ~Game();

    void run();
};

} // namespace holdem
