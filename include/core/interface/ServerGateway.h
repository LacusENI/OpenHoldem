#pragma once

#include "Event.hpp"

namespace poker {
/**
 * @brief 客户端出站：向服务器发送命令
 * @details **不带身份**——是哪个客户端，由绑定到该通道的服务器侧适配器决定。
 */
class ServerGateway {
public:
    virtual ~ServerGateway() = default;

    virtual void send(const GameCommand& command) = 0;
};
} // namespace poker
