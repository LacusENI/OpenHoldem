#pragma once

#include <concepts>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "Card.hpp"
#include "GameTypes.hpp"
#include "HandValue.hpp"
#include "Player.hpp"

namespace poker {

/**
 * @brief 枚举走 magic_enum，其他类型走静态工厂 `T::from_string`。
 * @details 两类实现分开，不再需要 `from_string<T>` 模板特化。
 */
template <typename T>
std::optional<T> parse_string(std::string_view str) {
    if constexpr (std::is_enum_v<T>) {
        return magic_enum::enum_cast<T>(str);
    } else {
        return T::from_string(str);
    }
}

template <typename T>
concept HasToString = requires(const T& value) {
    { to_string(value) } -> std::convertible_to<std::string>;
};

/** @brief 枚举：to_string + magic_enum::enum_cast。 */
template <typename T>
concept EnumConvertible =
    HasToString<T> && std::is_enum_v<T> && requires(std::string_view str) {
        { magic_enum::enum_cast<T>(str) } -> std::same_as<std::optional<T>>;
    };

/** @brief 非枚举：to_string + 静态工厂 T::from_string。 */
template <typename T>
concept FactoryConvertible = HasToString<T> && requires(std::string_view str) {
    { T::from_string(str) } -> std::same_as<std::optional<T>>;
};

template <typename T>
concept StringConvertible = EnumConvertible<T> || FactoryConvertible<T>;

template <typename T>
struct GeneralSerializer {
    static void to_json(nlohmann::json& j, const T& value) { j = to_string(value); }

    static void from_json(const nlohmann::json& j, T& value) {
        std::string s = j.get<std::string>();
        auto opt = parse_string<T>(s);
        if (!opt) {
            throw std::runtime_error("Deserialization failed: " + j.dump());
        }
        value = *opt;
    }
};
} // namespace poker

namespace nlohmann {
/** @brief 自动为所有 StringConvertible 类型提供 adl_serializer。 */
template <poker::StringConvertible T>
struct adl_serializer<T> : poker::GeneralSerializer<T> {};
} // namespace nlohmann
