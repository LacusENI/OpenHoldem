#ifndef OPENHOLDEM_I_GAME_VIEW_H
#define OPENHOLDEM_I_GAME_VIEW_H
#include <vector>

#include "GameModel.h"

namespace holdem {

struct OnGameStartedData {
    Position btn_position;
};

struct OnRoundStartedData {
    const GameModel& game_model;
};

struct OnPlayerTurnData {
    const Position position;
};

struct PlayerInputData {
    Action action;
};

struct OnPlayerActedData {
    const Action& action;
};

struct OnRoundEndedData {

};

struct OnShowdownCompletedData {
    const GameModel& game_model;
    const std::vector<std::pair<Position, HandValue>>& results;
};

struct OnWinnerDeclaredData {
    const std::vector<Position>& winners;
    const std::vector<Stack>& amounts;
};

struct OnGameOverData {
    const GameModel& game_model;
};

class IGameView {
public:
    virtual ~IGameView() = default;

    virtual void onGameStarted(const OnGameStartedData&data) const = 0;

    virtual void onRoundStarted(const OnRoundStartedData& data) const = 0;

    virtual PlayerInputData onPlayerTurn(const OnPlayerTurnData& data) const = 0;

    virtual void onPlayerActed(const OnPlayerActedData& data) const = 0;

    virtual void onRoundEnded(const OnRoundEndedData& data) const = 0;

    virtual void onShowdownCompleted(
        const OnShowdownCompletedData& data) const = 0;

    virtual void onWinnerDeclared(
        const OnWinnerDeclaredData& data) const = 0;

    virtual void onGameOver(const OnGameOverData& data) const = 0;
};


}

#endif //OPENHOLDEM_I_GAME_VIEW_H