#pragma once

#include <nlohmann/json.hpp>

#include "Event.hpp"
#include "GeneralSerializer.hpp"

namespace poker::Event {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NewPlayerJoined, new_seat, player_count)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerJoinRequest, player_name)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerJoinResponse, player_seat)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameStarted, btn_seat, big_blind, player_count)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsDealt, player_seat, hole_cards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StageChanged, stage)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlindPosted, seat, amount, is_big_blind, pot, player_chips,
                                   player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CommunityCardsDealt, community_cards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BettingRoundStarted, stage)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BettingRoundEnded, stage)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AwaitPlayerAction, player_seat, player_chips, player_bet,
                                   round_bet, min_raise_delta)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerActionSubmitted, target_bet, is_fold)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerActed, actor_seat, action, pot, player_chips, player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsRevealed, active_players, hole_cards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameEnded, winners, awards)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerChipsUpdated, player_seat, player_chips)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PotUpdated, pot)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerBetUpdated, player_seat, player_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoundBetUpdated, round_bet)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MinRaiseDeltaUpdated, min_raise_delta)
} // namespace poker::Event
namespace poker {
inline nlohmann::json serializeEvent(const GameEvent& event) {
    nlohmann::json j;
    std::visit(
        [&j]<typename T>(const T& e) {
            j = nlohmann::json{
                {"event", std::string(Event::get_event_name(std::type_identity<T>()))},
                {"data", e}};
        },
        event);
    return j;
}

inline GameEvent deserializeEvent(const nlohmann::json& j) {
    std::string type = j.at("event").get<std::string>();
    const nlohmann::json& data = j.at("data");
    if (type == "PlayerJoinRequest") return data.get<Event::PlayerJoinRequest>();
    if (type == "NewPlayerJoined") return data.get<Event::NewPlayerJoined>();
    if (type == "PlayerJoinResponse") return data.get<Event::PlayerJoinResponse>();
    if (type == "GameStarted") return data.get<Event::GameStarted>();
    if (type == "HoleCardsDealt") return data.get<Event::HoleCardsDealt>();
    if (type == "StageChanged") return data.get<Event::StageChanged>();
    if (type == "BlindPosted") return data.get<Event::BlindPosted>();
    if (type == "CommunityCardsDealt") return data.get<Event::CommunityCardsDealt>();
    if (type == "BettingRoundStarted") return data.get<Event::BettingRoundStarted>();
    if (type == "BettingRoundEnded") return data.get<Event::BettingRoundEnded>();
    if (type == "AwaitPlayerAction") return data.get<Event::AwaitPlayerAction>();
    if (type == "PlayerActionSubmitted") return data.get<Event::PlayerActionSubmitted>();
    if (type == "PlayerActed") return data.get<Event::PlayerActed>();
    if (type == "HoleCardsRevealed") return data.get<Event::HoleCardsRevealed>();
    if (type == "GameEnded") return data.get<Event::GameEnded>();
    throw std::runtime_error("Unknown event: " + type);
}
} // namespace poker
