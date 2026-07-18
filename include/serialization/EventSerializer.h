#pragma once

#include "Event.hpp"

namespace poker {
std::string serializeEvent(const GameEvent& event);

std::optional<GameEvent> deserializeEvent(const std::string& json_str);
} // namespace poker
