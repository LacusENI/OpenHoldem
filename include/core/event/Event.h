#pragma once
#include <variant>
#include <vector>

#include "Card.h"
#include "GameTypes.h"
#include "HandValue.h"

namespace poker {
namespace Event {
struct GameStarted {};

struct HoleCardsDealt {
    seat_idx_t player_seat;
    cards_2_t hole_cards;
};

struct StageChanged {
    Stage stage;
};

struct BlindPosted {
    seat_idx_t seat;
    chips_t amount;
    bool is_big_blind;
    chips_t pot;
    chips_t player_chips;
    chips_t player_bet;
};

struct CommunityCardsDealt {
    cards_5_t community_cards;
};

struct BettingRoundStarted {};

struct BettingRoundEnded {};

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
    Action action;
    chips_t pot;
    chips_t player_chips;
    chips_t player_bet;
};

struct HoleCardsRevealed {
    std::vector<seat_idx_t> active_players;
    std::vector<cards_2_t> hole_cards;
    std::vector<HandValue> hand_values;
};

struct WinnerDeclared {
    std::vector<seat_idx_t> winners;
    std::vector<chips_t> awards;
};

struct GameEnded {};

struct PlayerChipsUpdated {
    seat_idx_t player_seat;
    chips_t player_chips;
};

struct PotUpdated {
    chips_t pot;
};

struct PlayerBetUpdated {
    seat_idx_t player_seat;
    chips_t player_bet;
};

struct RoundBetUpdated {
    chips_t round_bet;
};

struct MinRaiseDeltaUpdated {
    chips_t min_raise_delta;
};

} // namespace Event
using GameEvent = std::variant<Event::GameStarted, Event::GameEnded>;
} // namespace poker