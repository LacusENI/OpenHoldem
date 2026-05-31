#include "GameModel.h"
#include "PlayerSet.h"
#include "BettingRound.h"
#include "Deck.h"
#include "PotManager.h"

namespace poker {
GameModel::GameModel(
    std::unique_ptr<IDeck> deck,
    std::shared_ptr<PlayerSet> players,
    std::shared_ptr<PotManager> pot_manager,
    std::unique_ptr<BettingRound> betting_round
    ) : deck(std::move(deck)), players(std::move(players)),
        pot_manager(std::move(pot_manager)),
        betting_round(std::move(betting_round)) {}

GameModel::~GameModel() = default;

void GameModel::startGame() {
    game_state = GameState::IDLE;
    deck->shuffle();
    pot_manager->clearPot();

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

void GameModel::distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        Player& player = players->at(winners[i]);
        chips_t amount = amounts[i];
        player.chips += amount;
    }
    pot_manager->clearPot();
}

void GameModel::nextStreet() {
    seat_idx_t current_position = 0;
    switch (game_state) {
    case GameState::IDLE:
        game_state = GameState::PREFLOP;
        current_position = players->getUtgPosition(btn_position);
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
}
}
