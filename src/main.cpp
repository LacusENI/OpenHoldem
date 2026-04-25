#include <iostream>

#include "entity/Card.h"
#include "entity/Deck.h"
#include "GameModel.h"
#include "Game.h"
#include "ConsoleView.h"
#include "Formatter.h"

using namespace holdem;

int main() {
    std::cout << ui::formatter::format(Card("CA"));
    std::cout << ui::formatter::format(Card("DJ"));
    std::cout << ui::formatter::format(Card("HT"));
    std::cout << ui::formatter::format(Card("S8"));
    std::cout << " OpenHoldem\n";

    auto deck = std::make_unique<Deck>();
    auto game_model = std::make_unique<GameModel>(std::move(deck));
    Game game(std::move(game_model), std::make_unique<ui::ConsoleView>());
    game.model->addPlayer();
    game.model->addPlayer();
    game.model->addPlayer();
    game.model->addPlayer();
    game.model->addPlayer();
    game.run();

    return 0;
}