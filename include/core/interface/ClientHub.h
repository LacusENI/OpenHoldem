#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "GameTypes.hpp"
#include "IClient.h"

namespace poker {
/**
 * @brief 服务器 → 客户端的扇出/单传
 * @details 暂时兼管 client 的注册生命周期（add/remove/find）。
 *          只保存**非拥有**的 IClient*；对象由组合根/会话管理器拥有。
 *          分发用成员函数指针，调用点仍保留 `IClient::onXXX` 的显式签名。
 */
class ClientHub {
public:
    void add(seat_t seat, IClient& client) {
        assert(seat >= 0 && "ClientHub::add: 非法座位");
        if (static_cast<std::size_t>(seat) >= clients_.size()) {
            clients_.resize(static_cast<std::size_t>(seat) + 1, nullptr);
        }
        assert(clients_[seat] == nullptr && "ClientHub::add: 座位已被占用");
        clients_[seat] = &client;
    }

    void remove(seat_t seat) {
        if (seat >= 0 && static_cast<std::size_t>(seat) < clients_.size()) {
            clients_[seat] = nullptr;
        }
    }

    [[nodiscard]] IClient* find(seat_t seat) const {
        if (seat < 0 || static_cast<std::size_t>(seat) >= clients_.size()) return nullptr;
        return clients_[seat];
    }

    [[nodiscard]] const std::vector<IClient*>& clients() const { return clients_; }

    template <typename Event>
    void broadcast(void (IClient::*method)(const Event&), const Event& event) const {
        // 快照：投递期间 add/remove 不会破坏遍历（对象本身仍需由外部保证存活）
        const std::vector<IClient*> snapshot = clients_;
        for (IClient* client : snapshot) {
            if (client != nullptr) (client->*method)(event);
        }
    }

    template <typename Event>
    void sendTo(seat_t seat, void (IClient::*method)(const Event&), const Event& event) const {
        IClient* client = find(seat);
        assert(client != nullptr && "ClientHub::sendTo: 目标座位没有客户端");
        if (client != nullptr) (client->*method)(event);
    }

private:
    std::vector<IClient*> clients_;
};
} // namespace poker
