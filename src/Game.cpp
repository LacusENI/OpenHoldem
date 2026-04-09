#include "Game.h"

#include <iostream>

#include "HandEvaluator.h"

namespace holdem {

void Game::run() {
    // 初始化
    if (game_state != GameState::INIT) {
        game_state = GameState::INIT;
        winners.clear();
    }
    // 打印庄家位信息
    PlayerId button = getPlayer(getButtonPosition()).id;
    output(std::format("Button: @P{}\n", button));

    // 进入 Preflop 阶段
    nextStreet();
    // 依次经过 Flop/Turn/River 阶段
    while (game_state != GameState::SHOWDOWN) {
        displayBoard();
        runBettingRound();
        nextStreet();
    }
    // 摊牌阶段
    showdown();
    displayBoard();
    award();
}

void Game::preflop() {
    // 洗牌
    deck->shuffle();
    // 发底牌
    for (Player& player : players) {
        player.hole_cards[0] = deck->deal();
        player.hole_cards[1] = deck->deal();
    }
    // 盲注
    Player& sb = getPlayer(getSmallBlindPosition());
    Player& bb = getPlayer(getBigBlindPosition());
    Stack small_blind = big_blind / 2;
    commitChips(sb.position, small_blind);
    commitChips(bb.position, big_blind);
    output(std::format("#[@P{}]: Small Blind (-${})\n", sb.id, small_blind));
    output(std::format("#[@P{}]: Big Blind (-${})\n", bb.id, big_blind));
}

void Game::takeAction() {
    Player& player = getPlayer(current_position);
    std::string action_msg;
    Stack amount = 0;
    if (current_bet == 0) {
        action_msg = "Bet";
        amount = big_blind;
    } else if (player.current_bet < current_bet) {
        action_msg = "Call";
        amount = current_bet - player.current_bet;
    } else {
        action_msg = "Check";
    }
    commitChips(current_position, amount);
    output(std::format("#[@P{}]: {} (-${})\n",
        player.id, action_msg, amount));
}

void Game::nextPlayer() {
    current_position = nextPosition(current_position);
    if (current_position == rest_position)
        round_ended = true;
}

void Game::award() {
    int winners_n = static_cast<int>(winners.size());
    int share = pot / winners_n;
    int remainder = pot % winners_n;

    output("Winner:");
    for (size_t i = 0; i < winners_n; ++i) {
        int amount = share;
        // 前 remainder 个玩家多得 1 个筹码
        if (i < remainder) amount += 1;

        Player& player = getPlayer(winners[i]);
        player.chips += amount;
        output(std::format(" @P{}(+${})", player.id, amount));
    }
    output("\n");

    // 打印最终结果
    output("Result\n");
    for (Player& player : players) {
        output(std::format("[@P{}] ${:>3}\n", player.id, player.chips));
    }

}

void Game::nextStreet() {
    for (Player& player : players) {
        player.current_bet = 0;
    }

    switch (game_state) {
    case GameState::INIT:
        game_state = GameState::PREFLOP;
        rest_position = getUtgPosition();
        preflop();
        break;
    case GameState::PREFLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        game_state = GameState::FLOP;
        rest_position = nextPosition(button_position);
        break;
    case GameState::FLOP:
        community_cards[3] = deck->deal();
        game_state = GameState::TURN;
        rest_position = nextPosition(button_position);
        break;
    case GameState::TURN:
        community_cards[4] = deck->deal();
        game_state = GameState::RIVER;
        rest_position = nextPosition(button_position);
        break;
    case GameState::RIVER:
        game_state = GameState::SHOWDOWN;
    default: break;
    }
    current_position = rest_position;
    round_ended = false;
    current_bet = 0;
}

void Game::showdown() {
    // 评估每位玩家的手牌大小
    for (Player& player : players) {
        Cards7 hand7 = concatCards(player.hole_cards, community_cards);
        player.hand_value = HandEvaluator::evaluate(hand7);
    }
    // 决出手牌最大的获胜者
    HandValue winner_value;
    for (Player& player : players) {
        if (player.hand_value == winner_value) {
            winners.push_back(player.position);
        }
        else if (player.hand_value > winner_value) {
            winners.clear();
            winners.push_back(player.position);
            winner_value = player.hand_value;
        }
    }
}

void Game::runBettingRound() {
    while (!round_ended) {
        takeAction();
        nextPlayer();
    }
}

void Game::displayBoard() {
    // 显示当前下注轮，底池
    std::string street_msg;
    switch (game_state) {
    case GameState::SHOWDOWN:
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
    output(std::format("---{}--- Pot: ${}\n", street_msg, pot));

    // 显示公共牌信息
    std::string cc1, cc2, cc3, cc4, cc5;
    switch (game_state) {
    case GameState::SHOWDOWN:
    case GameState::RIVER:
        cc5 = community_cards[4].toMessage();
    case GameState::TURN:
        cc4 = community_cards[3].toMessage();
    case GameState::FLOP:
        cc3 = community_cards[2].toMessage();
        cc2 = community_cards[1].toMessage();
        cc1 = community_cards[0].toMessage();
    case GameState::PREFLOP:
        break;
    default:
        throw std::runtime_error("Invalid Game State");
    }
    output(std::format("Community Cards: {} {} {} {} {}\n",
        cc1, cc2, cc3, cc4, cc5));

    // 显示每名玩家底牌信息
    output("Hold Cards:\n");
    for (Player& player : players) {
        PlayerId id = player.id;
        std::string hole1 = player.hole_cards[0].toMessage();
        std::string hole2 = player.hole_cards[1].toMessage();
        std::string hand_message;
        if (game_state == GameState::SHOWDOWN) {
            hand_message = internal::getHandMessage(player.hand_value);
        } else {
            hand_message = "";
        }
       output(std::format(
            "[@P{}] ${:<3} |  {} {}   {}\n",
            id, player.chips,hole1, hole2, hand_message));
    }
}

namespace internal {
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
} // namespace holdem
