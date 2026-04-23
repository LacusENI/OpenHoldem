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
    void onGameStarted(const OnGameStartedData&data) const override;
    void onRoundStarted(const OnRoundStartedData& data) const override;
    void onPlayerActed(const OnPlayerActedData& data) const override;
    PlayerInputData onPlayerTurn(const OnPlayerTurnData& data) const override;
    void onRoundEnded(const OnRoundEndedData& data) const override;
    void onShowdownCompleted(
        const OnShowdownCompletedData& data) const override;
    void onWinnerDeclared(
        const OnWinnerDeclaredData& data) const override;
    void onGameOver(const OnGameOverData& data) const override;
};
}
#endif //OPENHOLDEM_CONSOLE_VIEW_H