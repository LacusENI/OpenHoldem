#pragma once

#include "Event.hpp"

namespace poker {
/**
 * @brief 客户端入站：接收服务器事件
 * @details 单入口 `on(GameEvent)`；需要按事件类型处理的实现，
 *          可在内部用 `std::visit` 分发给自己的 `handle` 重载。
 */
class ClientHandler {
public:
    virtual ~ClientHandler() = default;

    virtual void on(const GameEvent& event) = 0;
};

/** @brief 空实现：机器人 / 测试 / 占位。 */
class NullClient : public ClientHandler {
public:
    void on(const GameEvent&) override {}
};
} // namespace poker
