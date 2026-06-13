#pragma once
#include <variant>

#include "Card.h"
#include "GameTypes.h"

namespace poker {
namespace Event {
struct GameStarted {
    seat_idx_t btn_seat;
};

struct HoleCardsDealt {
    seat_idx_t player_seat;
    cards_2_t hole_cards;
};

struct StreetStarted {
    Street street;
};

struct BlindPosted {
    seat_idx_t seat;
    chips_t amount;
    bool is_big_blind;
};

struct CommunityCardsDealt {
    cards_5_t community_cards;
};

struct BettingRoundStarted {
    seat_idx_t first_actor_seat;
};

struct GameEnded {
    chips_t pot;
};

} // namespace Event
using GameEvent = std::variant<Event::GameStarted, Event::GameEnded>;
} // namespace poker