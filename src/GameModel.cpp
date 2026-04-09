#include "GameModel.h"

namespace holdem {
void GameModel::addPlayer() {
    Player new_player;
    new_player.position = static_cast<int>(players.size());
    players.push_back(new_player);
}

void GameModel::commitChips(Position position, Stack amount) {
    Player& player = getPlayer(position);
    player.chips -= amount;
    pot += amount;
    player.current_bet += amount;
    if (player.current_bet > round_bet) {
        round_bet = player.current_bet;
    }
}

void GameModel::dealCards() {
    switch (game_state) {
    case GameState::PREFLOP:
        for (Player& player : players) {
            player.hole_cards[0] = deck->deal();
            player.hole_cards[1] = deck->deal();
        }
        break;
    case GameState::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case GameState::TURN:
        community_cards[3] = deck->deal();
        break;
    case GameState::RIVER:
        community_cards[4] = deck->deal();
        break;
    default: break;
    }
}
}