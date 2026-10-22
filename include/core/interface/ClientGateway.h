#pragma once

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
/**
 * @brief 服务器出站：向客户端发送事件
 * @details 公开事件用 `broadcast`，私密 / 定向用 `sendTo`（按座位）。
 */
class ClientGateway {
public:
    virtual ~ClientGateway() = default;

    virtual void broadcast(const GameEvent& event) = 0;
    virtual void sendTo(seat_t seat, const GameEvent& event) = 0;
};
} // namespace poker
