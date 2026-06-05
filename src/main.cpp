#include <iostream>

#include "spdlog/spdlog.h"

#include "BettingRound.h"
#include "Card.h"
#include "Deck.h"
#include "GameModel.h"
#include "GameController.h"
#include "Formatter.h"
#include "PotManager.h"
#include "SeatingManager.h"

using namespace poker;

int main() {
    std::cout << ui::formatter::format(Card("CA"));
    std::cout << ui::formatter::format(Card("DJ"));
    std::cout << ui::formatter::format(Card("HT"));
    std::cout << ui::formatter::format(Card("S8"));
    std::cout << " OpenHoldem\n";

    auto deck = std::make_unique<Deck>();
    auto seating_manager = std::make_shared<SeatingManager>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>(seating_manager, pot_manager);
    auto game_model = std::make_unique<GameModel>(
        std::move(deck), pot_manager, seating_manager, std::move(betting_round));
    GameController game(std::move(game_model));
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    seating_manager->addNewPlayer();
    spdlog::info("game started");
    game.run();

    return 0;
}