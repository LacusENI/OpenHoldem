#ifndef OPENHOLDEM_GAME_H
#define OPENHOLDEM_GAME_H
#include <memory>

#include "GameModel.h"
#include "GameView.h"

namespace holdem {
/**
 * @brief 负责控制一局游戏的流程
 */
class Game {
public:
    GameModel model;
    std::unique_ptr<IGameView> view;

    explicit Game(
        std::unique_ptr<IDeck> deck,
        std::unique_ptr<IGameView> view) : model(std::move(deck)), view(std::move(view)) {}

    void run();

    void addPlayer(PlayerId id) {
        model.addPlayer(id);
    }

    void nextStreet() {
        model.nextStreet();
    }

    Player& getPlayer(Position pos) {
        return model.getPlayer(pos);
    }

    void handleAward();
};

} // namespace holdem

#endif //OPENHOLDEM_GAME_H