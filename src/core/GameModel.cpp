#include "GameModel.h"
#include "BettingRound.h"
#include "Deck.h"
#include "SeatingManager.h"
#include "PotManager.h"

namespace poker {
GameModel::GameModel(
    std::unique_ptr<IDeck> deck,
    std::shared_ptr<PotManager> pot_manager,
    std::shared_ptr<SeatingManager> seating_manager,
    std::unique_ptr<BettingRound> betting_round
    ) : deck(std::move(deck)), seating_manager(std::move(seating_manager)),
        pot_manager(std::move(pot_manager)),
        betting_round(std::move(betting_round)) {}

GameModel::~GameModel() = default;

void GameModel::startGame() {
    game_state = Street::IDLE;
    deck->shuffle();
    pot_manager->clearPot();

}

void GameModel::dealCards() {
    switch (game_state) {
    case Street::PREFLOP:
        for (Player& player : seating_manager->getPlayerList()) {
            player.hole_cards[0] = deck->deal();
            player.hole_cards[1] = deck->deal();
        }
        break;
    case Street::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case Street::TURN:
        community_cards[3] = deck->deal();
        break;
    case Street::RIVER:
        community_cards[4] = deck->deal();
        break;
    default: break;
    }
}

void GameModel::distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        Player& player = seating_manager->getPlayerAtSeat(winners[i]);
        chips_t amount = amounts[i];
        player.chips += amount;
    }
    pot_manager->clearPot();
}

void GameModel::nextStreet() {
    seat_idx_t current_position = 0;
    switch (game_state) {
    case Street::IDLE:
        game_state = Street::PREFLOP;
        current_position = seating_manager->getPreflopFirstPosition();
        break;
    case Street::PREFLOP:
        game_state = Street::FLOP;
        current_position = seating_manager->getPostflopFirstPosition();
        break;
    case Street::FLOP:
        game_state = Street::TURN;
        current_position = seating_manager->getPostflopFirstPosition();
        break;
    case Street::TURN:
        game_state = Street::RIVER;
        current_position = seating_manager->getPostflopFirstPosition();
        break;
    case Street::RIVER:
        game_state = Street::AWARD;
    default: break;
    }
    dealCards();
}
} // namespace poker
