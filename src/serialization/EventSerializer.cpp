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
