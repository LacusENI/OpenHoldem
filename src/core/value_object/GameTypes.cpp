#include "GameTypes.h"

std::string poker::to_string(Street street) {
    switch (street) {
    case Street::IDLE:
        return "IDLE";
    case Street::PREFLOP:
        return "PREFLOP";
    case Street::FLOP:
        return "FLOP";
    case Street::TURN:
        return "TURN";
    case Street::RIVER:
        return "RIVER";
    default:
        return "???";
    }
}
