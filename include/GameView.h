#ifndef OPENHOLDEM_GAME_VIEW_H
#define OPENHOLDEM_GAME_VIEW_H
#include <vector>

#include "GameModel.h"
#include "HandEvaluator.h"

namespace holdem {
class IGameView {
public:

    virtual ~IGameView() = default;

    virtual void onGameStarted() = 0;

    virtual void onRoundStarted(const GameModel& game_model) = 0;

    virtual void onPlayerActed(const Action& action) = 0;

    virtual void onRoundEnded() = 0;

    virtual void onShowdownCompleted(const GameModel& game_model, const std::vector<HandValue>& results) = 0;

    virtual void onWinnerDeclared(const std::vector<Position>& winners, const std::vector<Stack>& amounts) = 0;

    virtual void onGameOver(const GameModel& game_model) = 0;
};

class GameView final : public IGameView {
private:
    static void output(const std::string& text);
    static void displayBoard(const GameModel& game_model, const std::vector<std::string>& hand_msgs);
public:
    void onGameStarted() override;
    void onRoundStarted(const GameModel& game_model) override;
    void onPlayerActed(const Action& action) override;
    void onRoundEnded() override;
    void onShowdownCompleted(const GameModel& game_model, const std::vector<HandValue>& results) override;
    void onWinnerDeclared(const std::vector<Position>& winners, const std::vector<Stack>& amounts) override;
    void onGameOver(const GameModel& game_model) override;
};

}

#endif //OPENHOLDEM_GAME_VIEW_H