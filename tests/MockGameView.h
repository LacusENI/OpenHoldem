#ifndef OPENHOLDEM_MOCK_GAME_VIEW_H
#define OPENHOLDEM_MOCK_GAME_VIEW_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IGameView.h"

using namespace holdem;

class MockGameView : public IGameView {
public:
    MOCK_METHOD(void, onGameStarted, (const OnGameStartedData&), (const, override));
    MOCK_METHOD(void, onRoundStarted, (const OnRoundStartedData&), (const, override));
    MOCK_METHOD(PlayerInputData, onPlayerTurn, (const OnPlayerTurnData&), (const, override));
    MOCK_METHOD(void, onPlayerActed, (const OnPlayerActedData&), (const, override));
    MOCK_METHOD(void, onRoundEnded, (const OnRoundEndedData&), (const, override));
    MOCK_METHOD(void, onShowdownCompleted, (const OnShowdownCompletedData&), (const, override));
    MOCK_METHOD(void, onWinnerDeclared, (const OnWinnerDeclaredData&), (const, override));
    MOCK_METHOD(void, onGameOver, (const OnGameOverData&), (const, override));
};

#endif //OPENHOLDEM_MOCK_GAME_VIEW_H