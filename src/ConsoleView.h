#ifndef OPENHOLDEM_CONSOLE_VIEW_H
#define OPENHOLDEM_CONSOLE_VIEW_H
#include "IGameView.h"

namespace holdem {
class ConsoleView final : public IGameView {
private:
    static void output(const std::string& text);
    static void displayBoard(
        const GameModel& game_model,
        const std::vector<std::pair<Position, std::string>>& active_player_hand_msgs
        );
public:
    void onGameStarted() override;
    void onRoundStarted(const GameModel& game_model) override;
    void onPlayerActed(const Action& action) override;
    Action onPlayerTurn(const Position& position) override;
    void onRoundEnded() override;
    void onShowdownCompleted(
        const GameModel& game_model, const std::vector<std::pair<Position, HandValue>>& results) override;
    void onWinnerDeclared(
        const std::vector<Position>& winners, const std::vector<Stack>& amounts) override;
    void onGameOver(const GameModel& game_model) override;
};
}
#endif //OPENHOLDEM_CONSOLE_VIEW_H