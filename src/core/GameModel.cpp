#include "GameModel.h"
#include "PlayerSet.h"
#include "BettingRound.h"
#include "Deck.h"
#include "PotManager.h"

namespace holdem {
GameModel::GameModel(
    std::unique_ptr<IDeck> deck,
    std::shared_ptr<PlayerSet> players,
    std::shared_ptr<PotManager> pot_manager,
    std::unique_ptr<BettingRound> betting_round
    ) : deck(std::move(deck)), players(std::move(players)),
        pot_manager(std::move(pot_manager)),
        betting_round(std::move(betting_round)) {}

GameModel::~GameModel() = default;

Position GameModel::getSmallBlindPosition() const  {
    return players->nextPosition(btn_position);
}

Position GameModel::getBigBlindPosition() const  {
    return players->nextPosition(getSmallBlindPosition());
}

void GameModel::commitChips(Position position, Stack amount) {
    betting_round->commitChips(position, amount);
    pot_manager->addChipsToPot(amount);
}

void GameModel::setup() {
    game_state = GameState::IDLE;
    pot_manager->clearPot();
    deck->shuffle();
}

void GameModel::dealCards() {
    switch (game_state) {
    case GameState::PREFLOP:
        for (Player& player : *players) {
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
    auto [position, action_type, amount] = betting_round->handleAction(action, big_blind);
    commitChips(position, amount);
    return {position, action_type, amount};
}

void GameModel::nextActor() {
    betting_round->nextTurn();
}

void GameModel::distributePot(const std::vector<Stack>& amounts, const std::vector<Position>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        Player& player = players->at(winners[i]);
        Stack amount = amounts[i];
        player.chips += amount;
    }
    pot_manager->clearPot();
}

void GameModel::nextStreet() {
    Position current_position = 0;
    switch (game_state) {
    case GameState::IDLE:
        game_state = GameState::PREFLOP;
        current_position = players->nextPosition(getBigBlindPosition());
        break;
    case GameState::PREFLOP:
        game_state = GameState::FLOP;
        current_position = players->nextPositionToAct(btn_position);
        break;
    case GameState::FLOP:
        game_state = GameState::TURN;
        current_position = players->nextPositionToAct(btn_position);
        break;
    case GameState::TURN:
        game_state = GameState::RIVER;
        current_position = players->nextPositionToAct(btn_position);
        break;
    case GameState::RIVER:
        game_state = GameState::AWARD;
    default: break;
    }
    dealCards();
    betting_round->prepare(current_position);
}
}
