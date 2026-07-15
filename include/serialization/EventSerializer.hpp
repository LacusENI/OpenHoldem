#pragma once

#include <nlohmann/json.hpp>

#include "Event.hpp"
#include "GeneralSerializer.hpp"

namespace poker::Event {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerJoined, new_seat, player_count)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerJoinSucceeded, player_seat)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameStarted, btn_seat, big_blind, player_count)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsDealt, player_seat, hole_cards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StageChanged, stage)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlindPosted, seat, amount, is_big_blind, pot, player_chips,
                                   player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CommunityCardsDealt, community_cards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AwaitPlayerInput, player_seat, player_chips, player_bet,
                                   round_bet, min_raise_delta)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerInputSubmitted, target_bet, is_fold)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerActed, actor_seat, action, pot, player_chips, player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsRevealed, active_players, hole_cards, hand_values)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WinnerDeclared, winners, awards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerChipsUpdated, player_seat, player_chips)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PotUpdated, pot)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerBetUpdated, player_seat, player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoundBetUpdated, round_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MinRaiseDeltaUpdated, min_raise_delta)

namespace poker {
inline void to_json(nlohmann::json& j, const PokerEvent& v) {
    std::visit(
        [&j]<typename T>(const T& inner_event) {
            using EventType = std::decay_t<T>;

            // 写入事件类型名
            j["event"] = get_event_name(std::type_identity<EventType>{});

            // 写入事件数据（使用 ADL 找到的 to_json）
            nlohmann::json data_obj;
            // 这里需要调用通用的 to_json，让 ADL 找到具体类型的版本
            to_json(data_obj, inner_event);
            j["data"] = std::move(data_obj);
        },
        v);
}
} // namespace poker

} // namespace poker::Event
