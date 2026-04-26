#include <iostream>

#include "BettingRound.h"
#include "Card.h"
#include "Deck.h"
#include "GameModel.h"
#include "Game.h"
#include "ConsoleView.h"
#include "Formatter.h"
#include "PlayerSet.h"
#include "PotManager.h"

using namespace holdem;

int main() {
    std::cout << ui::formatter::format(Card("CA"));
    std::cout << ui::formatter::format(Card("DJ"));
    std::cout << ui::formatter::format(Card("HT"));
    std::cout << ui::formatter::format(Card("S8"));
    std::cout << " OpenHoldem\n";

    auto deck = std::make_unique<Deck>();
    auto player_set = std::make_shared<PlayerSet>();
    auto pot_manager = std::make_shared<PotManager>();
    auto betting_round = std::make_unique<BettingRound>(player_set);
    auto game_model = std::make_unique<GameModel>(
        std::move(deck), player_set, pot_manager, std::move(betting_round));
    Game game(std::move(game_model), std::make_unique<ui::ConsoleView>());
    player_set->addNewPlayer();
    player_set->addNewPlayer();
    player_set->addNewPlayer();
    player_set->addNewPlayer();
    player_set->addNewPlayer();
    game.run();

    return 0;
}