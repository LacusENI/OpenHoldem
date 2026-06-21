#pragma once
#include <variant>
#include <vector>

#include "Card.h"
#include "GameTypes.h"
#include "HandValue.h"

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

struct AwaitPlayerInput {
    seat_idx_t actor_seat;
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
};

struct PlayerInputSubmitted {
    chips_t target_bet;
    bool is_fold;
};

struct PlayerActed {
    seat_idx_t actor_seat;
    ActionType action_type;
};

struct ShowdownCompleted {
    std::vector<seat_idx_t> active_players;
    std::vector<cards_2_t> hole_cards;
    std::vector<HandValue> hand_values;
};

struct WinnerDeclared {
    std::vector<std::pair<seat_idx_t, chips_t>> winners_and_awards;
};

struct GameEnded {
    chips_t pot;
};

} // namespace Event
using GameEvent = std::variant<Event::GameStarted, Event::GameEnded>;
} // namespace poker