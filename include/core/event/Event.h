#pragma once
#include <variant>

#include "GameTypes.h"

namespace poker {
namespace Event {
struct GameStarted {
    seat_idx_t btn_seat;
};

struct GameEnded {
    chips_t pot;
};

} // namespace Event
using GameEvent = std::variant<Event::GameStarted, Event::GameEnded>;
} // namespace poker