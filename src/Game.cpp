#include "Game.h"

#include "HandEvaluator.h"

namespace holdem {

void Game::run() {
    if (game_state != GameState::INIT) {
        throw std::runtime_error("Game state is not init");
    }
    deck->shuffle();
    dealHoleCards();
    while (game_state != GameState::SHOWDOWN) {
        render();
        nextStreet();
    }
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
    for (Player& player : players) {
        Cards7 hand7 = concatCards(player.hole_cards, community_cards);
        player.hand_value = HandEvaluator::evaluate(hand7);
    }
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

void Game::render() {} // TODO: 待实现
}
