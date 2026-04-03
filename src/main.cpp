#include <iostream>

#include "Card.h"

int main() {
    std::cout << holdem::Card("CA").toMessage();
    std::cout << holdem::Card("DJ").toMessage();
    std::cout << holdem::Card("HT").toMessage();
    std::cout << holdem::Card("S8").toMessage();
    std::cout << "\n";
    std::cout << "OpenHoldem\n";
    std::cout << "Hello World!\n";
    return 0;
}