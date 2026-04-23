#include <iostream>

#include "entity/Card.h"
#include "entity/Deck.h"
#include "GameModel.h"
#include "Game.h"
#include "ConsoleView.h"

using namespace holdem;

int main() {
    std::cout << Card("CA").toMessage();
    std::cout << Card("DJ").toMessage();
    std::cout << Card("HT").toMessage();
    std::cout << Card("S8").toMessage();
    std::cout << " OpenHoldem\n";

    auto deck = std::make_unique<Deck>();
    auto game_model = std::make_unique<GameModel>(std::move(deck));
    Game game(std::move(game_model), std::make_unique<ConsoleView>());
    game.model->addPlayer(1);
    game.model->addPlayer(2);
    game.model->addPlayer(3);
    game.model->addPlayer(4);
    game.model->addPlayer(5);
    game.run();

    return 0;
}