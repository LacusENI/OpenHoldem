#ifndef OPENHOLDEM_GAME_H
#define OPENHOLDEM_GAME_H
#include <iostream>
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
        if (model.print_enabled) std::cout << msg;
    }

    void displayAction(const Action& action) const {
        if (model.print_enabled)
            output(std::format("#[@P{}]: {} (-${})\n",
                action.actor_position + 1, action.action_name, action.amount));
    }
};

} // namespace holdem

#endif //OPENHOLDEM_GAME_H