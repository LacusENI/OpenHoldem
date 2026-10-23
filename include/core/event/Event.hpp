#pragma once

#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "Card.hpp"
#include "GameTypes.hpp"

namespace poker {

/**
 * 包含 GameEvent 的类型列表的 X-marco
 * 用法：
 *   #define MY_DECL(Type) ...
 *   POKER_EVENT_TYPES(MY_DECL)
 *   #undef MY_DECL
 */
#define POKER_EVENT_TYPES(X) \
    X(PlayerJoined)          \
    X(JoinResponse)          \
    X(GameStarted)           \
    X(HoleCardsDealt)        \
    X(StageChanged)          \
    X(BlindPosted)           \
    X(CommunityCardsDealt)   \
    X(TurnStarted)           \
    X(PlayerActed)           \
    X(HoleCardsRevealed)     \
    X(PotAwarded)            \
    X(UncalledBetReturned)   \
    X(GameEnded)

#define POKER_COMMAND_TYPES(X) \
    X(JoinRequest)             \
    X(StartGame)               \
    X(SubmitAction)

#define POKER_NETWORK_EVENT_TYPES(X) \
    X(ClientConnected)               \
    X(ClientDisconnected)            \
    X(ServerDisconnected)

// ===== 客户端 → 服务器 =====
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

#define POKER_DEFINE_COMMAND_NAME(Type) \
    constexpr std::string_view get_event_name(std::type_identity<Type>) { return #Type; }
POKER_COMMAND_TYPES(POKER_DEFINE_COMMAND_NAME)
#undef POKER_DEFINE_COMMAND_NAME
} // namespace Command

// ===== 服务器 → 客户端 =====
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

struct GameStarted {
    seat_t btn_seat;   // 按钮位
    chips_t big_blind; // 大盲注
    int player_count;  // 玩家数量
};

struct HoleCardsDealt {
    seat_t player_seat;
    cards_2_t hole_cards;
};

struct StageChanged {
    Stage stage;
};

struct BlindPosted {
    seat_t player;
    chips_t amount;
    bool is_big_blind;
};

struct CommunityCardsDealt {
    cards_5_t community_cards;
};

struct TurnStarted {
    seat_t player;
    chips_t player_chips;
    chips_t player_bet;
    chips_t round_bet;
    chips_t min_raise;
    bool can_raise;
};

struct PlayerActed {
    seat_t player;
    chips_t bet;
    Action action;
    bool is_all_in;
};

struct HoleCardsRevealed {
    std::vector<std::pair<seat_t, cards_2_t>> players_with_hole_cards;
};

struct PotAwarded {
    int pot_index;
    chips_t chips;
    std::vector<seat_t> award_to;
};

struct UncalledBetReturned {
    seat_t player;
    chips_t amount;
};

struct GameEnded {};

#define POKER_DEFINE_EVENT_NAME(Type) \
    constexpr std::string_view get_event_name(std::type_identity<Type>) { return #Type; }
POKER_EVENT_TYPES(POKER_DEFINE_EVENT_NAME)
#undef POKER_DEFINE_EVENT_NAME
} // namespace Event

// ===== 网络层通知 =====
namespace NetworkEvent {
struct ClientConnected {
    int client_id;
};

struct ClientDisconnected {
    int client_id;
};

struct ServerDisconnected {};

#define POKER_DEFINE_NETWORK_EVENT_NAME(Type) \
    constexpr std::string_view get_event_name(std::type_identity<Type>) { return #Type; }
POKER_NETWORK_EVENT_TYPES(POKER_DEFINE_NETWORK_EVENT_NAME)
#undef POKER_DEFINE_NETWORK_EVENT_NAME
} // namespace NetworkEvent

// ===== 由列表生成 variant =====
namespace detail {
template <typename... Ts>
struct TypeList {};

template <typename List>
struct VariantFromList;

/* 列表以 void 开头，只是为了避开模板实参列表不能有尾逗号的问题。 */
template <typename... Ts>
struct VariantFromList<TypeList<void, Ts...>> {
    using type = std::variant<Ts...>;
};

using EventTypeList = TypeList<void
#define POKER_EVENT_TYPELIST_ENTRY(Type) , Event::Type
    POKER_EVENT_TYPES(POKER_EVENT_TYPELIST_ENTRY)
#undef POKER_EVENT_TYPELIST_ENTRY
>;

using CommandTypeList = TypeList<void
#define POKER_COMMAND_TYPELIST_ENTRY(Type) , Command::Type
    POKER_COMMAND_TYPES(POKER_COMMAND_TYPELIST_ENTRY)
#undef POKER_COMMAND_TYPELIST_ENTRY
>;

using NetworkEventTypeList = TypeList<void
#define POKER_NETWORK_TYPELIST_ENTRY(Type) , NetworkEvent::Type
    POKER_NETWORK_EVENT_TYPES(POKER_NETWORK_TYPELIST_ENTRY)
#undef POKER_NETWORK_TYPELIST_ENTRY
>;
} // namespace detail

using GameEvent = typename detail::VariantFromList<detail::EventTypeList>::type;
using GameCommand = typename detail::VariantFromList<detail::CommandTypeList>::type;
using NetworkEventVariant = typename detail::VariantFromList<detail::NetworkEventTypeList>::type;

} // namespace poker
