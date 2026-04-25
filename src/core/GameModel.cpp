#include "GameModel.h"
#include "Deck.h"

namespace holdem {
GameModel::GameModel(std::unique_ptr<IDeck> deck) : deck(std::move(deck)) {}

GameModel::~GameModel() = default;

void GameModel::commitChips(Position position, Stack amount) {
    Player& player = players.at(position);
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
    for (Player& player : players) {
        player.current_bet = 0;
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
    return {position, ActionType::BIG_BLIND, big_blind};
}

Action GameModel::smallBlind() {
    Position position = getSmallBlindPosition();
    Stack small_blind = big_blind / 2;
    commitChips(position, small_blind);
    return {position, ActionType::SMALL_BLIND, small_blind};
}

Action GameModel::takeAction(const Action& action) {
    Player& player = players.at(current_position);
    if (action.type == ActionType::FOLD) {
        player.is_folded = true;
        return {current_position, ActionType::FOLD, 0};
    }
    ActionType action_type;
    Stack amount = 0;
    if (round_bet == 0) {
        action_type = ActionType::BET;
        amount = big_blind;
    } else if (player.current_bet < round_bet) {
        action_type = ActionType::CALL;
        amount = round_bet - player.current_bet;
    } else {
        action_type = ActionType::CHECK;
    }
    commitChips(current_position, amount);
    return {current_position, action_type, amount};
}

void GameModel::nextActor() {
    Position next_position = players.nextPositionToAct(current_position);
    is_round_ended =
        next_position == rest_position ||
            players.nextPositionToAct(next_position) == next_position;
    is_only_one_active = players.nextPositionToAct(next_position) == next_position;
    if (players.at(current_position).is_folded
        && current_position == rest_position) {
        rest_position = next_position;
    }
    current_position = next_position;
}

void GameModel::distributePot(const std::vector<Stack>& amounts, const std::vector<Position>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        Player& player = players.at(winners[i]);
        Stack amount = amounts[i];
        player.chips += amount;
    }
    pot = 0;
}

void GameModel::nextStreet() {
    for (Player& player : players) {
        player.current_bet = 0;
    }

    switch (game_state) {
    case GameState::IDLE:
        game_state = GameState::PREFLOP;
        current_position = players.nextPosition(getBigBlindPosition());
        break;
    case GameState::PREFLOP:
        game_state = GameState::FLOP;
        current_position = players.nextPositionToAct(btn_position);
        break;
    case GameState::FLOP:
        game_state = GameState::TURN;
        current_position = players.nextPositionToAct(btn_position);
        break;
    case GameState::TURN:
        game_state = GameState::RIVER;
        current_position = players.nextPositionToAct(btn_position);
        break;
    case GameState::RIVER:
        game_state = GameState::AWARD;
    default: break;
    }
    dealCards();
    rest_position = current_position;
    if (!is_only_one_active)
        is_round_ended = false;
    round_bet = 0;
}
}