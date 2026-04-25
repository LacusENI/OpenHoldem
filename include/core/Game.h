#pragma once

#include <memory>

namespace holdem {
class GameModel;
class IGameView;

/**
 * @brief 负责控制一局游戏的流程
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

    void handleAward();
};

} // namespace holdem
