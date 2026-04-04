#include <iostream>

#include "Card.h"
#include "Game.h"

using namespace holdem;

int main() {
    std::cout << Card("CA").toMessage();
    std::cout << Card("DJ").toMessage();
    std::cout << Card("HT").toMessage();
    std::cout << Card("S8").toMessage();
    std::cout << " OpenHoldem\n";

    Game game(std::move(std::make_unique<Deck>()));
    game.addPlayer(1);
    game.addPlayer(2);
    game.addPlayer(3);
    game.addPlayer(4);
    game.addPlayer(5);
    game.run();

    return 0;
}