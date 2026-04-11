#include "Game.h"
#include "ShowdownHandler.h"

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
} // namespace holdem::internal

namespace holdem {
void Game::run() {
    // 初始化
    model.setup();
    // 打印庄家位信息
    PlayerId button = getPlayer(model.button_position).id;
    output(std::format("Button: @P{}\n", button));

    // 进入 Preflop 阶段
    nextStreet();

    // 执行盲注
    Action sb = model.smallBlind();
    Action bb = model.bigBlind();
    displayAction(sb);
    displayAction(bb);

    // 依次经过 Flop/Turn/River 阶段
    while (model.game_state != GameState::AWARD) {
        displayBoard();
        while (!model.is_round_ended) {
            Action action = model.takeAction();
            displayAction(action);
            model.nextPlayer();
        }
        nextStreet();
    }
    // 摊牌阶段
    auto amounts = handleAward();
    displayBoard();

    // 打印分配底池结果
    output("Winner:");
    for (int i = 0; i < amounts.size(); ++i) {
        Stack amount = amounts[i];
        const Player& player = model.getPlayer(model.winners[i]);
        output(std::format(" @P{}(+${})", player.id, amount));
    }
    output("\n");

    // 打印最终结果
    output("Result\n");
    for (Player& player : model.players) {
        output(std::format("[@P{}] ${:>3}\n", player.id, player.chips));
    }
}

void Game::displayBoard() {
    // 显示当前下注轮，底池
    std::string street_msg;
    switch (model.game_state) {
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
    output(std::format("---{}--- Pot: ${}\n", street_msg, model.pot));

    // 显示公共牌信息
    std::string cc1, cc2, cc3, cc4, cc5;
    switch (model.game_state) {
    case GameState::AWARD:
    case GameState::RIVER:
        cc5 = model.community_cards[4].toMessage();
    case GameState::TURN:
        cc4 = model.community_cards[3].toMessage();
    case GameState::FLOP:
        cc3 = model.community_cards[2].toMessage();
        cc2 = model.community_cards[1].toMessage();
        cc1 = model.community_cards[0].toMessage();
    case GameState::PREFLOP:
        break;
    default:
        throw std::runtime_error("Invalid Game State");
    }
    output(std::format("Community Cards: {} {} {} {} {}\n",
        cc1, cc2, cc3, cc4, cc5));

    // 显示每名玩家底牌信息
    output("Hold Cards:\n");
    for (const Player& player : model.players) {
        PlayerId id = player.id;
        std::string hole1 = player.hole_cards[0].toMessage();
        std::string hole2 = player.hole_cards[1].toMessage();
        std::string hand_message;
        if (model.game_state == GameState::AWARD) {
            hand_message = internal::getHandMessage(player.hand_value);
        } else {
            hand_message = "";
        }
        output(std::format(
             "[@P{}] ${:<3} |  {} {}   {}\n",
             id, player.chips,hole1, hole2, hand_message));
    }
}

std::vector<Stack> Game::handleAward() {
    auto hand_values = ShowdownHandler::evalHandValues(
        model.players, model.community_cards);
    model.winners = ShowdownHandler::determineWinners(
        hand_values);
    auto amounts = ShowdownHandler::calculateDistribution(
        model.pot, model.winners.size());
    model.distributePot(amounts);
    return amounts;
}
}
