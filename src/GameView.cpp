#include <iostream>

#include "GameView.h"

namespace holdem::internal {
std::string rankToMessage(Rank rank);
std::string getHandMessage(HandValue hand_value);
} // namespace holdem::internal

namespace holdem {
void GameView::output(const std::string& text) {
    std::cout << text;
}

void GameView::displayBoard(const GameModel& game_model, const std::vector<std::string>& hand_msgs) {
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
        cc5 = game_model.community_cards[4].toMessage();
    case GameState::TURN:
        cc4 = game_model.community_cards[3].toMessage();
    case GameState::FLOP:
        cc3 = game_model.community_cards[2].toMessage();
        cc2 = game_model.community_cards[1].toMessage();
        cc1 = game_model.community_cards[0].toMessage();
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
        std::string hole1 = player.hole_cards[0].toMessage();
        std::string hole2 = player.hole_cards[1].toMessage();
        std::string hand_message = i < hand_msgs.size() ? hand_msgs[i] : "";
        output(std::format(
             "[@P{}] ${:<3} |  {} {}   {}\n",
             id, player.chips, hole1, hole2, hand_message));
    }
}

void GameView::onGameStarted() {}

void GameView::onRoundStarted(const GameModel& game_model) {
    displayBoard(game_model, {});
}

void GameView::onPlayerActed(const Action& action) {
    output(std::format("#[@P{}]: {} (-${})\n",
            action.actor_position + 1, action.description, action.amount));
}

Action GameView::onPlayerTurn(const Position& position) {
    std::cout << "[@P" << position + 1 << "]Continue Or Fold [C/f]: ";
    std::string input;
    std::cin >> input;
    if (input == "f" && input == "F") {
        return Action(position, "Fold", 0);
    }
    return Action(position, "", 0);
}

void GameView::onRoundEnded() {}

void GameView::onShowdownCompleted(const GameModel& game_model, const std::vector<HandValue>& results) {
    std::vector<std::string> hand_msgs;
    for (const HandValue& hv : results) {
        hand_msgs.push_back(internal::getHandMessage(hv));
    }
    displayBoard(game_model, hand_msgs);
}

void GameView::onWinnerDeclared(const std::vector<Position>& winners, const std::vector<Stack>& amounts) {
    output("Winner:");
    for (int i = 0; i < winners.size(); ++i) {
        Position position = winners[i];
        Stack amount = amounts[i];
        output(std::format(" @P{}(+${})", position + 1, amount));
    }
    output("\n");
}

void GameView::onGameOver(const GameModel& game_model) {
    output("Result\n");
    for (const Player& player : game_model.players) {
        output(std::format("[@P{}] ${:>3}\n", player.position + 1, player.chips));
    }
}
}

namespace holdem::internal {
std::string rankToMessage(Rank rank) {
    switch (rank) {
    case Rank::ACE: return "Ace";
    case Rank::KING: return "King";
    case Rank::QUEEN: return "Queen";
    case Rank::JACK: return "Jack";
    case Rank::TEN: return "Ten";
    default:
        return std::to_string(static_cast<int>(rank));
    }
}
std::string getHandMessage(HandValue hand_value) {
    HandType hand_type = hand_value.getHandType();
    std::string x, y;
    switch (hand_type) {
    case HandType::ROYAL_FLUSH:
        return "Royal Flush";
    case HandType::STRAIGHT_FLUSH:
        x = rankToMessage(hand_value.getRank(4));
        y = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Straight Flush, {} to {}", x, y);
    case HandType::FOUR_OF_A_KIND:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Four of a Kind, {}s", x);
    case HandType::FULL_HOUSE:
        x = rankToMessage(hand_value.getRank(0));
        y = rankToMessage(hand_value.getRank(1));
        return std::format(
            "Full House, {}s full of {}s", x, y);
    case HandType::FLUSH:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Flush, {}-high", x);
    case HandType::STRAIGHT:
        x = rankToMessage(hand_value.getRank(4));
        y = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Straight, {} to {}", x, y);
    case HandType::THREE_OF_A_KIND:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "Three of a Kind, {}s", x);
    case HandType::TWO_PAIR:
        x = rankToMessage(hand_value.getRank(0));
        y = rankToMessage(hand_value.getRank(1));
        return std::format(
            "Two pair, {}s and {}s", x, y);
    case HandType::ONE_PAIR:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "One Pair, {}s", x);
    case HandType::HIGH_CARD:
        x = rankToMessage(hand_value.getRank(0));
        return std::format(
            "High Card, {}-high", x);
    }
    return "";
}
}