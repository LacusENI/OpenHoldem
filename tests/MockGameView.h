#ifndef OPENHOLDEM_MOCK_GAME_VIEW_H
#define OPENHOLDEM_MOCK_GAME_VIEW_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "GameView.h"

using namespace holdem;

using ShowdownResult = std::vector<std::pair<Position, HandValue>>;

class MockGameView : public IGameView {
public:
    MOCK_METHOD(void, onGameStarted, (), (override));
    MOCK_METHOD(void, onRoundStarted, (const GameModel& game_model), (override));
    MOCK_METHOD(Action, onPlayerTurn, (const Position& position), (override));
    MOCK_METHOD(void, onPlayerActed, (const Action& action), (override));
    MOCK_METHOD(void, onRoundEnded, (), (override));
    MOCK_METHOD(void, onShowdownCompleted,
        (const GameModel& game_model, const ShowdownResult& results), (override));
    MOCK_METHOD(void, onWinnerDeclared, (const std::vector<Position>& winners, const std::vector<Stack>& amounts), (override));
    MOCK_METHOD(void, onGameOver, (const GameModel& game_model), (override));
};

#endif //OPENHOLDEM_MOCK_GAME_VIEW_H