#include "EventSerializer.h"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include <nlohmann/json.hpp>

#include "Event.hpp"
#include "GeneralSerializer.hpp"

namespace poker::Event {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinResponse, uid, seat)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerJoined, uid, seat, player_count)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameStarted, btn_seat, big_blind, player_count)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsDealt, player_seat, hole_cards)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StageChanged, stage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlindPosted, player, amount, is_big_blind)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CommunityCardsDealt, community_cards)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TurnStarted, player, player_chips, player_bet, round_bet,
                                   min_raise, can_raise)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerActed, player, bet, action, is_all_in)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HoleCardsRevealed, players_with_hole_cards)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PotAwarded, pot_index, chips, award_to)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UncalledBetReturned, player, amount)

/* GameEnded 没有字段，显式给一个空对象，避免把无参宏交给 NLOHMANN 定义。 */
inline void to_json(nlohmann::json& j, const GameEnded&) { j = nlohmann::json::object(); }
inline void from_json(const nlohmann::json&, GameEnded&) {}
} // namespace poker::Event

namespace poker::Command {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JoinRequest, uid)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StartGame, big_blind, initial_chips)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SubmitAction, target_bet, is_fold)
} // namespace poker::Command

namespace poker {
namespace {

/* 用 ADL 找到 Event::get_event_name / Command::get_event_name。 */
template <typename T>
constexpr std::string_view type_name() {
    return get_event_name(std::type_identity<T>{});
}

template <typename Variant>
std::string serialize_variant(const Variant& value, std::string_view key) {
    nlohmann::json j;
    std::visit(
        [&j, key]<typename T>(const T& event) {
            j = nlohmann::json{
                {std::string(key), std::string(type_name<T>())},
                {"data", event},
            };
        },
        value);
    return j.dump();
}

template <typename Variant, std::size_t Index>
void try_deserialize(const std::string& name, const nlohmann::json& data,
                     std::optional<Variant>& out) {
    using Alternative = std::variant_alternative_t<Index, Variant>;
    if (!out.has_value() && name == type_name<Alternative>()) {
        out = data.get<Alternative>();
    }
}

template <typename Variant, std::size_t... Index>
std::optional<Variant> deserialize_variant_impl(const std::string& name,
                                                const nlohmann::json& data,
                                                std::index_sequence<Index...>) {
    std::optional<Variant> out;
    (try_deserialize<Variant, Index>(name, data, out), ...);
    return out;
}

template <typename Variant>
std::optional<Variant> deserialize_variant(const std::string& json_str, std::string_view key) {
    try {
        const nlohmann::json j = nlohmann::json::parse(json_str);
        const std::string name = j.at(std::string(key)).get<std::string>();
        const nlohmann::json& data = j.at("data");
        return deserialize_variant_impl<Variant>(
            name, data, std::make_index_sequence<std::variant_size_v<Variant>>{});
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

} // namespace

std::string serializeEvent(const GameEvent& event) {
    return serialize_variant(event, "event");
}

std::optional<GameEvent> deserializeEvent(const std::string& json_str) {
    return deserialize_variant<GameEvent>(json_str, "event");
}

std::string serializeCommand(const GameCommand& command) {
    return serialize_variant(command, "command");
}

std::optional<GameCommand> deserializeCommand(const std::string& json_str) {
    return deserialize_variant<GameCommand>(json_str, "command");
}
} // namespace poker
