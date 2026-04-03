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
    game.addPlayer(Player());
    game.addPlayer(Player());
    game.addPlayer(Player());
    game.run();

    return 0;
}