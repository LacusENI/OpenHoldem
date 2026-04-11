#include "Card.h"
#include "GameModel.h"
#include "HandEvaluator.h"

namespace holdem {
void GameModel::addPlayer(PlayerId id) {
    Player new_player;
    new_player.id = id;
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

void GameModel::setup() {
    game_state = GameState::IDLE;
    pot = 0;
    round_bet = 0;
    is_round_ended = false;
    winners.clear();
    for (Player& player : players) {
        player.current_bet = 0;
        player.hand_value = HandValue();
    }
    deck->shuffle();
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

Action GameModel::bigBlind() {
    Position position = getBigBlindPosition();
    commitChips(position, big_blind);
    return {position, "Big Blind", big_blind};
}

Action GameModel::smallBlind() {
    Position position = getSmallBlindPosition();
    Stack small_blind = big_blind / 2;
    commitChips(position, small_blind);
    return {position, "Small Blind", small_blind};
}

Action GameModel::takeAction() {
    Player& player = getPlayer(current_position);
    std::string action_msg;
    Stack amount = 0;
    if (round_bet == 0) {
        action_msg = "Bet";
        amount = big_blind;
    } else if (player.current_bet < round_bet) {
        action_msg = "Call";
        amount = round_bet - player.current_bet;
    } else {
        action_msg = "Check";
    }
    commitChips(current_position, amount);
    return {current_position, action_msg, amount};
}

void GameModel::nextPlayer() {
    current_position = nextPosition(current_position);
    if (current_position == rest_position)
        is_round_ended = true;
}

void GameModel::distributePot(std::vector<Stack> amounts) {
    for (int i = 0; i < amounts.size(); i++) {
        Player& player = getPlayer(winners[i]);
        Stack amount = amounts[i];
        player.chips += amount;
    }
}

void GameModel::nextStreet() {
    for (Player& player : players) {
        player.current_bet = 0;
    }

    switch (game_state) {
    case GameState::IDLE:
        game_state = GameState::PREFLOP;
        current_position = getUtgPosition();
        break;
    case GameState::PREFLOP:
        game_state = GameState::FLOP;
        current_position = nextPosition(button_position);
        break;
    case GameState::FLOP:
        game_state = GameState::TURN;
        current_position = nextPosition(button_position);
        break;
    case GameState::TURN:
        game_state = GameState::RIVER;
        current_position = nextPosition(button_position);
        break;
    case GameState::RIVER:
        game_state = GameState::AWARD;
    default: break;
    }
    dealCards();
    rest_position = current_position;
    is_round_ended = false;
    round_bet = 0;
}
}