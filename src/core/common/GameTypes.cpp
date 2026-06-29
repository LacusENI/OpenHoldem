#include "GameTypes.h"
namespace poker {

Stage next_stage(Stage stage) {
    switch (stage) {
    case Stage::IDLE:
        return Stage::PREFLOP;
    case Stage::PREFLOP:
        return Stage::FLOP;
    case Stage::FLOP:
        return Stage::TURN;
    case Stage::TURN:
        return Stage::RIVER;
    case Stage::RIVER:
        return Stage::SHOWDOWN;
    case Stage::SHOWDOWN:
        return Stage::SETTLEMENT;
    default:
        return Stage::IDLE;
    }
}

std::string to_string(Stage stage) {
    switch (stage) {
    case Stage::IDLE:
        return "IDLE";
    case Stage::PREFLOP:
        return "PREFLOP";
    case Stage::FLOP:
        return "FLOP";
    case Stage::TURN:
        return "TURN";
    case Stage::RIVER:
        return "RIVER";
    case Stage::SHOWDOWN:
        return "SHOWDOWN";
    case Stage::SETTLEMENT:
        return "SETTLEMENT";
    default:
        return "???";
    }
}

std::string to_string(Action action) {
    switch (action) {
    case Action::VOID:
        return "VOID";
    case Action::CALL:
        return "CALL";
    case Action::CHECK:
        return "CHECK";
    case Action::RAISE:
        return "RAISE";
    case Action::BET:
        return "BET";
    case Action::FOLD:
        return "FOLD";
    case Action::ALL_IN:
        return "ALL_IN";
    default:
        return "???";
    }
}
} // namespace poker