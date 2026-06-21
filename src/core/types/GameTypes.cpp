#include "GameTypes.h"
namespace poker {

Street next_street(Street street) {
    switch (street) {
    case Street::IDLE:
        return Street::PREFLOP;
    case Street::PREFLOP:
        return Street::FLOP;
    case Street::FLOP:
        return Street::TURN;
    case Street::TURN:
        return Street::RIVER;
    case Street::RIVER:
        return Street::AWARD;
    default:
        return Street::IDLE;
    }
}

std::string to_string(Street street) {
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
} // namespace poker