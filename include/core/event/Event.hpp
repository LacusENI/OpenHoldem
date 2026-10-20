#pragma once
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
namespace Command {
struct JoinRequest {
    unsigned uid = 0; // 用户ID（保留字段）
};
struct SubmitAction {
    chips_t target_bet;
    bool is_fold;
};
struct StartGame {
    chips_t big_blind;     // 大盲注金额
    chips_t initial_chips; // 每名玩家初始筹码
};
} // namespace Command

namespace Event {
struct JoinResponse {
    unsigned uid = 0; // 用户ID（保留字段）
    seat_t seat;      // 给玩家分配的座位号
};

struct PlayerJoined {
    unsigned uid = 0; // 用户ID（保留字段）
    seat_t seat;
    int player_count;
};

GET_EVENT_NAME(PlayerJoined)

struct GameStarted {
    seat_t btn_seat;   // 按钮位
    chips_t big_blind; // 大盲注
    int player_count;  // 玩家数量
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

struct TurnStarted {
    seat_t player;
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise;
    bool can_raise;
};

GET_EVENT_NAME(TurnStarted)

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

struct PotAwarded {
    int pot_index;
    chips_t chips;
    std::vector<seat_t> award_to;
};

struct UncalledBetReturned {
    seat_t player;
    chips_t amount;
};

GET_EVENT_NAME(UncalledBetReturned)

struct GameEnded {};

GET_EVENT_NAME(GameEnded)

} // namespace Event

using GameEvent = std::variant<Event::PlayerJoined, Event::JoinResponse, Event::GameStarted,
                               Event::HoleCardsDealt, Event::StageChanged, Event::BlindPosted,
                               Event::CommunityCardsDealt, Event::TurnStarted, Event::PlayerActed,
                               Event::HoleCardsRevealed, Event::PotAwarded,
                               Event::UncalledBetReturned, Event::GameEnded>;

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
