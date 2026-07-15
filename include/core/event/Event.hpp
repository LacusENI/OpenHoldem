#pragma once
#include <variant>
#include <vector>

#include "Card.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"

#define GET_EVENT_NAME(Type)                                              \
constexpr std::string_view get_event_name(std::type_identity<Type>) { \
return #Type;                                                     \
}

namespace poker {
namespace Event {
struct PlayerJoining {};

GET_EVENT_NAME(PlayerJoining)

struct PlayerJoined {
    seat_idx_t new_seat;
    int player_count;
};

GET_EVENT_NAME(PlayerJoined)

struct PlayerJoinSucceeded {
    seat_idx_t player_seat;
};

GET_EVENT_NAME(PlayerJoinSucceeded)

struct GameStarted {
    seat_idx_t btn_seat;
    chips_t big_blind;
    int player_count;
};

GET_EVENT_NAME(GameStarted)

struct HoleCardsDealt {
    seat_idx_t player_seat;
    cards_2_t hole_cards;
};

GET_EVENT_NAME(HoleCardsDealt)

struct StageChanged {
    Stage stage;
};

GET_EVENT_NAME(StageChanged)

struct BlindPosted {
    seat_idx_t seat;
    chips_t amount;
    bool is_big_blind;
    chips_t pot;
    chips_t player_chips;
    chips_t player_bet;
};

GET_EVENT_NAME(BlindPosted)

struct CommunityCardsDealt {
    cards_5_t community_cards;
};

GET_EVENT_NAME(CommunityCardsDealt)

struct BettingRoundStarted {};

GET_EVENT_NAME(BettingRoundStarted)

struct BettingRoundEnded {};

GET_EVENT_NAME(BettingRoundEnded)

struct AwaitPlayerInput {
    seat_idx_t player_seat;
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise_delta;
};

GET_EVENT_NAME(AwaitPlayerInput)

struct PlayerInputSubmitted {
    chips_t target_bet;
    bool is_fold;
};

GET_EVENT_NAME(PlayerInputSubmitted)

struct PlayerActed {
    seat_idx_t actor_seat;
    Action action;
    chips_t pot;
    chips_t player_chips;
    chips_t player_bet;
};

GET_EVENT_NAME(PlayerActed)

struct HoleCardsRevealed {
    std::vector<seat_idx_t> active_players;
    std::vector<cards_2_t> hole_cards;
    std::vector<HandValue> hand_values;
};

GET_EVENT_NAME(HoleCardsRevealed)

struct WinnerDeclared {
    std::vector<seat_idx_t> winners;
    std::vector<chips_t> awards;
};

GET_EVENT_NAME(WinnerDeclared)

struct GameEnded {};

GET_EVENT_NAME(GameEnded)

struct PlayerChipsUpdated {
    seat_idx_t player_seat;
    chips_t player_chips;
};

GET_EVENT_NAME(PlayerChipsUpdated)

struct PotUpdated {
    chips_t pot;
};

GET_EVENT_NAME(PotUpdated)

struct PlayerBetUpdated {
    seat_idx_t player_seat;
    chips_t player_bet;
};

GET_EVENT_NAME(PlayerBetUpdated)

struct RoundBetUpdated {
    chips_t round_bet;
};

GET_EVENT_NAME(RoundBetUpdated)

struct MinRaiseDeltaUpdated {
    chips_t min_raise_delta;
};

GET_EVENT_NAME(MinRaiseDeltaUpdated)

} // namespace Event
using PokerEvent =
    std::variant<Event::PlayerJoining, Event::PlayerJoined, Event::PlayerJoinSucceeded,
                 Event::GameStarted, Event::HoleCardsDealt, Event::StageChanged, Event::BlindPosted,
                 Event::CommunityCardsDealt, Event::BettingRoundStarted, Event::BettingRoundEnded,
                 Event::AwaitPlayerInput, Event::PlayerInputSubmitted, Event::PlayerActed,
                 Event::HoleCardsRevealed, Event::WinnerDeclared, Event::GameEnded,
                 Event::PlayerChipsUpdated, Event::PotUpdated, Event::PlayerBetUpdated,
                 Event::RoundBetUpdated, Event::MinRaiseDeltaUpdated>;
} // namespace poker