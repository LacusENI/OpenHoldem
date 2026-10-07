#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "Card.hpp"
#include "GameTypes.hpp"

#define GET_EVENT_NAME(Type)                                              \
    constexpr std::string_view get_event_name(std::type_identity<Type>) { \
        return #Type;                                                     \
    }

namespace poker {
namespace Event {
struct PlayerJoinRequest {
    std::string player_name;
};

GET_EVENT_NAME(PlayerJoinRequest)

struct NewPlayerJoined {
    seat_t new_seat;
    int player_count;
};

GET_EVENT_NAME(NewPlayerJoined)

struct PlayerJoinResponse {
    seat_t player_seat;
};

GET_EVENT_NAME(PlayerJoinResponse)

struct StartGame {};

GET_EVENT_NAME(StartGame)

struct GameStarted {
    seat_t btn_seat;
    chips_t big_blind;
    int player_count;
};

GET_EVENT_NAME(GameStarted)

struct HoleCardsDealt {
    seat_t player_seat;
    cards_2_t hole_cards;
};

GET_EVENT_NAME(HoleCardsDealt)

struct StageChanged {
    Stage stage;
};

GET_EVENT_NAME(StageChanged)

struct BlindPosted {
    seat_t player;
    chips_t amount;
    bool is_big_blind;
};

GET_EVENT_NAME(BlindPosted)

struct CommunityCardsDealt {
    cards_5_t community_cards;
};

GET_EVENT_NAME(CommunityCardsDealt)

struct BettingRoundStarted {
    Stage stage;
};

GET_EVENT_NAME(BettingRoundStarted)

struct BettingRoundEnded {
    Stage stage;
};

GET_EVENT_NAME(BettingRoundEnded)

struct WaitingForAction {
    seat_t player;
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise;
    bool can_raise;
};

GET_EVENT_NAME(WaitingForAction)

struct PlayerAction {
    chips_t target_bet;
    bool is_fold;
    bool is_all_in;
};

GET_EVENT_NAME(PlayerAction)

struct PlayerActed {
    seat_t player;
    chips_t bet;
    Action action;
    bool is_all_in;
};

GET_EVENT_NAME(PlayerActed)

struct HoleCardsRevealed {
    std::vector<std::pair<seat_t, cards_2_t>> players_with_hole_cards;
};

GET_EVENT_NAME(HoleCardsRevealed)

struct PotSplited {
    std::vector<chips_t> pot_chips;
};

GET_EVENT_NAME(PotSplited);

struct WinnerDeclared {
    int pot_index;
    std::vector<seat_t> winners;
};

GET_EVENT_NAME(WinnerDeclared)

struct UncalledBetReturned {
    seat_t player;
    chips_t amount;
};

GET_EVENT_NAME(UncalledBetReturned)

struct GameEnded {};

GET_EVENT_NAME(GameEnded)

} // namespace Event

using GameEvent =
    std::variant<Event::PlayerJoinRequest, Event::NewPlayerJoined, Event::PlayerJoinResponse,
                 Event::GameStarted, Event::HoleCardsDealt, Event::StageChanged,
                 Event::BlindPosted, Event::CommunityCardsDealt, Event::BettingRoundStarted,
                 Event::BettingRoundEnded, Event::WaitingForAction, Event::PlayerAction,
                 Event::PlayerActed, Event::HoleCardsRevealed, Event::PotSplited,
                 Event::WinnerDeclared, Event::UncalledBetReturned, Event::GameEnded>;

namespace NetworkEvent {
struct ClientConnected {
    int client_id;
};

GET_EVENT_NAME(ClientConnected)

struct ClientDisconnected {
    int client_id;
};

GET_EVENT_NAME(ClientDisconnected)

struct ServerDisconnected {};

GET_EVENT_NAME(ServerDisconnected)

} // namespace NetworkEvent
} // namespace poker

#undef GET_EVENT_NAME