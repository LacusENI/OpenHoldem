#pragma once

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
/**
 * @brief 服务器 → 客户端的出站端口
 * @details 路由写在事件产生处：`broadcast` 广播，`sendTo` 单传给指定座位。
 *          由应用层实现（网络 / 控制台 / 测试）；`core` 只依赖该抽象。
 */
class ClientGateway {
public:
    virtual ~ClientGateway() = default;
    virtual void broadcast(const GameEvent& event) = 0;
    virtual void sendTo(seat_t seat, const GameEvent& event) = 0;
};

/** @brief 空的出站实现：用于不需要出站的场合（占位 / 测试）。 */
class NullClientGateway final : public ClientGateway {
public:
    void broadcast(const GameEvent&) override {}
    void sendTo(seat_t, const GameEvent&) override {}
};
} // namespace poker
