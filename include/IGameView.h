#ifndef OPENHOLDEM_I_GAME_VIEW_H
#define OPENHOLDEM_I_GAME_VIEW_H
#include <vector>

#include "GameModel.h"

namespace holdem {
class IGameView {
public:
    virtual ~IGameView() = default;

    virtual void onGameStarted() = 0;

    virtual void onRoundStarted(const GameModel& game_model) = 0;

    virtual Action onPlayerTurn(const Position& position) = 0;

    virtual void onPlayerActed(const Action& action) = 0;

    virtual void onRoundEnded() = 0;

    virtual void onShowdownCompleted(
        const GameModel& game_model, const std::vector<std::pair<Position, HandValue>>& results) = 0;

    virtual void onWinnerDeclared(
        const std::vector<Position>& winners, const std::vector<Stack>& amounts) = 0;

    virtual void onGameOver(const GameModel& game_model) = 0;
};


}

#endif //OPENHOLDEM_I_GAME_VIEW_H