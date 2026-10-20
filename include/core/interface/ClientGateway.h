#pragma once

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class ClientGateway {
public:
    virtual ~ClientGateway() = default;
    virtual void boardcast(const GameEvent&) {}
    virtual void sendto(seat_t, const GameEvent&) {}
};
} // namespace poker