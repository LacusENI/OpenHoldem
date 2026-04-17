#ifndef OPENHOLDEM_SHOWDOWN_HANDLER_H
#define OPENHOLDEM_SHOWDOWN_HANDLER_H
#include <vector>
#include "GameModel.h"
#include "HandEvaluator.h"

namespace holdem {

class ShowdownHandler {
public:
    static std::vector<std::pair<Position, HandValue>> evalHandValues(
        const std::vector<Player>& players,
        const Cards5& community_cards);

    static std::vector<Position> determineWinners(
        const std::vector<std::pair<Position, HandValue>>& hand_values);

    static std::vector<Stack> calculateDistribution(
        int pot, int winners_n);
};

}
#endif //OPENHOLDEM_SHOWDOWN_HANDLER_H