#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "Card.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"
#include "Player.hpp"

namespace poker {

template <typename T>
std::optional<T> from_string(std::string_view str);

/**
 *
 * @tparam T 提供 to_string() 和 from_string() 的基础类型
 */
template <typename T>
struct GeneralSerializer {
    static void to_json(nlohmann::json& j, const T& value) { j = to_string(value); }

    static void from_json(const nlohmann::json& j, T& value) {
        std::string s = j.get<std::string>();
        auto opt = from_string<T>(s);
        if (!opt) {
            throw std::runtime_error("Deserialization failed: " + j.dump());
        }
        value = *opt;
    }
};
} // namespace poker

namespace nlohmann {
template <>
struct adl_serializer<poker::Suit> : poker::GeneralSerializer<poker::Suit> {};

template <>
struct adl_serializer<poker::Rank> : poker::GeneralSerializer<poker::Rank> {};

template <>
struct adl_serializer<poker::Card> : poker::GeneralSerializer<poker::Card> {};

template <>
struct adl_serializer<poker::Stage> : poker::GeneralSerializer<poker::Stage> {};

template <>
struct adl_serializer<poker::Action> : poker::GeneralSerializer<poker::Action> {};

template <>
struct adl_serializer<poker::HandValue> : poker::GeneralSerializer<poker::HandValue> {};

template <>
struct adl_serializer<poker::PlayerState> : poker::GeneralSerializer<poker::PlayerState> {};

} // namespace nlohmann