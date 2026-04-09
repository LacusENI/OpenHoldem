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

}