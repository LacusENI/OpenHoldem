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
    // 洗牌，发底牌
    deck->shuffle();
    dealHoleCards();
    // 依次经过 Flop/Turn/River 阶段
    while (game_state != GameState::SHOWDOWN) {
        render();
        nextStreet();
    }
    // 摊牌阶段
    showdown();
    render();
}

void Game::dealHoleCards() {
    if (game_state != GameState::INIT) {
        throw std::runtime_error("Game state is not init");
    }
    for (Player& player : players) {
        player.hole_cards[0] = deck->deal();
        player.hole_cards[1] = deck->deal();
    }
    game_state = GameState::PREFLOP;
}

void Game::nextStreet() {
    switch (game_state) {
    case GameState::PREFLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        game_state = GameState::FLOP;
        break;
    case GameState::FLOP:
        community_cards[3] = deck->deal();
        game_state = GameState::TURN;
        break;
    case GameState::TURN:
        community_cards[4] = deck->deal();
        game_state = GameState::SHOWDOWN;
        break;
    default:
        throw std::runtime_error("Invalid game state for nextStreet()");
    }
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
            winners.push_back(&player);
        }
        else if (player.hand_value > winner_value) {
            winners.clear();
            winners.push_back(&player);
            winner_value = player.hand_value;
        }
    }
}

void Game::render() {
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
    std::cout << std::format(
        "Community Cards: {} {} {} {} {}\n",
        cc1, cc2, cc3, cc4, cc5);

    // 显示每名玩家底牌信息
    std::cout << "Hold Cards:\n";
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
        std::cout << std::format(
            "[@P{}] {} {}   {}\n", id, hole1, hole2, hand_message);
    }

    // 若处于摊牌阶段，显示获胜的玩家
    if (game_state == GameState::SHOWDOWN) {
        std::cout << "Winner:";
        for (Player* player : winners) {
            std::cout << std::format(" @P{}", player->id);
        }
        std::cout << "\n";
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
