#pragma once

#include "Event.hpp"

namespace poker {
/**
 * @brief 客户端 → 服务器的入站端口
 * @details 客户端只发送意图/命令；座位由服务器从连接上下文得知，不由客户端声明。
 */
class ServerGateway {
public:
    virtual ~ServerGateway() = default;
    virtual void send(const Command::JoinRequest&) = 0;
    virtual void send(const Command::StartGame&) = 0;
    virtual void send(const Command::SubmitAction&) = 0;
};

/** @brief 空的入站实现：用于不需要发送命令的场合（占位 / 测试）。 */
class NullServerGateway final : public ServerGateway {
public:
    void send(const Command::JoinRequest&) override {}
    void send(const Command::StartGame&) override {}
    void send(const Command::SubmitAction&) override {}
};
} // namespace poker
