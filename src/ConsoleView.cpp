#include <iostream>
#include <format>

#include "ConsoleView.h"
#include "GameModel.h"
#include "Formatter.h"

namespace holdem::ui {
void ConsoleView::output(const std::string& text) {
    std::cout << text;
}

void ConsoleView::displayBoard(
        const GameModel& game_model,
        const std::vector<std::pair<Position, std::string>>& active_player_hand_msgs
    ) {
    std::vector<std::string> hand_msgs(game_model.players.size(), "");
    for (auto [position, msg] : active_player_hand_msgs) {
        hand_msgs[position] = msg;
    }
    // 显示当前下注轮，底池
    std::string street_msg;
    switch (game_model.game_state) {
    case GameState::AWARD:
        street_msg = "SHOWDOWN";
        break;
    case GameState::RIVER:
        street_msg = "RIVER";
        break;
    case GameState::TURN:
        street_msg = "TURN";
        break;
    case GameState::FLOP:
        street_msg = "FLOP";
        break;
    case GameState::PREFLOP:
        street_msg = "PREFLOP";
        break;
    default:
        throw std::runtime_error("Invalid Game State");
    }
    output(std::format("---{}--- Pot: ${}\n", street_msg, game_model.pot));

    // 显示公共牌信息
    std::string cc1, cc2, cc3, cc4, cc5;
    switch (game_model.game_state) {
    case GameState::AWARD:
    case GameState::RIVER:
        cc5 = ui::formatter::format(game_model.community_cards[4]);
    case GameState::TURN:
        cc4 = ui::formatter::format(game_model.community_cards[3]);
    case GameState::FLOP:
        cc3 = ui::formatter::format(game_model.community_cards[2]);
        cc2 = ui::formatter::format(game_model.community_cards[1]);
        cc1 = ui::formatter::format(game_model.community_cards[0]);
    case GameState::PREFLOP:
        break;
    default:
        throw std::runtime_error("Invalid Game State");
    }
    output(std::format("Community Cards: {} {} {} {} {}\n",
        cc1, cc2, cc3, cc4, cc5));

    // 显示每名玩家底牌信息
    output("Hold Cards:\n");
    for (size_t i = 0; i < game_model.players.size(); ++i) {
        const Player& player = game_model.players[i];
        PlayerId id = player.id;
        std::string hole1 = ui::formatter::format(player.hole_cards[0]);
        std::string hole2 = ui::formatter::format(player.hole_cards[1]);
        std::string hand_message = hand_msgs[i];
        output(std::format(
             "[@P{}] ${:<3} |  {} {}   {}\n",
             id, player.chips, hole1, hole2, hand_message));
    }
}

void ConsoleView::onGameStarted(const OnGameStartedData&data) const {}

void ConsoleView::onRoundStarted(const OnRoundStartedData& data) const {
    displayBoard(data.game_model, {});
}

void ConsoleView::onPlayerActed(const OnPlayerActedData& data) const {
    const Action& action = data.action;
    output(std::format("#[@P{}]: {} (-${})\n",
            action.actor_position + 1, ui::formatter::format(action.type), action.amount));
}

PlayerInputData ConsoleView::onPlayerTurn(const OnPlayerTurnData& data) const {
    Position position = data.position;
    std::cout << "[@P" << position + 1 << "]Continue Or Fold [C/f]: ";
    std::string input;
    std::cin >> input;
    ActionType action_type = ActionType::VOID;
    if (input == "f" || input == "F") {
        action_type = ActionType::FOLD;
    }
    return {Action(position, action_type, 0)};
}

void ConsoleView::onRoundEnded(const OnRoundEndedData& data) const {}

void ConsoleView::onShowdownCompleted(const OnShowdownCompletedData& data) const {
    auto& results = data.results;
    std::vector<std::pair<Position, std::string>> hand_msgs;
    for (auto [position, hand_value] : results) {
        std::string hand_msg = ui::formatter::format(hand_value);
        hand_msgs.emplace_back(position, hand_msg);
    }
    displayBoard(data.game_model, hand_msgs);
}

void ConsoleView::onWinnerDeclared(const OnWinnerDeclaredData& data) const {
    output("Winner:");
    for (int i = 0; i < data.winners.size(); ++i) {
        Position position = data.winners[i];
        Stack amount = data.amounts[i];
        output(std::format(" @P{}(+${})", position + 1, amount));
    }
    output("\n");
}

void ConsoleView::onGameOver(const OnGameOverData& data) const {
    output("Result\n");
    for (const Player& player : data.game_model.players) {
        output(std::format("[@P{}] ${:>3}\n", player.position + 1, player.chips));
    }
}
}
