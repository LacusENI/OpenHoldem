#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "ClientGateway.h"
#include "ClientHandler.h"
#include "GameTypes.hpp"

namespace poker {
/**
 * @brief 服务器出站扇出：`ClientGateway` 的本地实现 + 座位登记
 * @details 按座位保存**非拥有**的 `ClientHandler*`；对象由组合根 / 会话管理器拥有。
 */
class ClientHub : public ClientGateway {
public:
    void add(seat_t seat, ClientHandler& client) {
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

    [[nodiscard]] ClientHandler* find(seat_t seat) const {
        if (seat < 0 || static_cast<std::size_t>(seat) >= clients_.size()) return nullptr;
        return clients_[seat];
    }

    /** @brief 反查某客户端的座位；未登记返回 INVALID_SEAT。 */
    [[nodiscard]] seat_t seatOf(const ClientHandler& client) const {
        for (std::size_t i = 0; i < clients_.size(); ++i) {
            if (clients_[i] == &client) return static_cast<seat_t>(i);
        }
        return INVALID_SEAT;
    }

    [[nodiscard]] const std::vector<ClientHandler*>& clients() const { return clients_; }

    void broadcast(const GameEvent& event) override {
        // 快照：投递期间 add/remove 不会破坏遍历（对象本身仍需由外部保证存活）
        const std::vector<ClientHandler*> snapshot = clients_;
        for (ClientHandler* client : snapshot) {
            if (client != nullptr) client->on(event);
        }
    }

    void sendTo(seat_t seat, const GameEvent& event) override {
        ClientHandler* client = find(seat);
        assert(client != nullptr && "ClientHub::sendTo: 目标座位没有客户端");
        if (client != nullptr) client->on(event);
    }

private:
    std::vector<ClientHandler*> clients_;
};
} // namespace poker
