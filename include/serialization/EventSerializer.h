#pragma once

#include <optional>
#include <string>

#include "Event.hpp"

namespace poker {
/** @brief 服务器→客户端事件：{"event": <类型名>, "data": {...}} */
std::string serializeEvent(const GameEvent& event);
std::optional<GameEvent> deserializeEvent(const std::string& json_str);

/** @brief 客户端→服务器命令：{"command": <类型名>, "data": {...}} */
std::string serializeCommand(const GameCommand& command);
std::optional<GameCommand> deserializeCommand(const std::string& json_str);
} // namespace poker
