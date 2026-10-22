#pragma once

#include "Event.hpp"

namespace poker {
class IClient;

/**
 * @brief 客户端 → 服务器的命令接收接口
 * @details `onJoin` 需要携带 reply endpoint（`client`）：服务器分配座位后
 *          把它登记进 ClientHub，后续该座位的事件才能发回来。
 */
class IServer {
public:
    virtual ~IServer() = default;

    virtual void onJoin(IClient& client, const Command::JoinRequest&) = 0;
    virtual void onStart(const Command::StartGame&) = 0;
    virtual void onSubmit(const Command::SubmitAction&) = 0;
};
} // namespace poker
