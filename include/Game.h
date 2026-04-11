#ifndef OPENHOLDEM_GAME_H
#define OPENHOLDEM_GAME_H
#include <memory>
#include "GameModel.h"

namespace holdem {
/**
 * @brief 负责控制一局游戏的流程
 */
class Game {
public:
    GameModel model;

    explicit Game(std::unique_ptr<IDeck> deck) : model(std::move(deck)) {}

    void run();

    void displayBoard();

    void addPlayer(PlayerId id) {
        model.addPlayer(id);
    }

    void nextStreet() {
        model.nextStreet();
    }

    Player& getPlayer(Position pos) {
        return model.getPlayer(pos);
    }

    void showdown() {
        model.showdown();
    }

    void output(const std::string& msg) const {
        model.output(msg);
    }

    void displayAction() {

    }
};

} // namespace holdem

#endif //OPENHOLDEM_GAME_H