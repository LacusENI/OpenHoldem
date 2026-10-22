#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <queue>

#include "ClientHandler.h"
#include "GameTypes.hpp"
#include "ServerGateway.h"
#include "ServerHandler.h"

namespace poker {
/**
 * @brief 本地回环：异步、双向、FIFO
 * @details
 *   - `serverSide()` 是 `ClientHandler`：服务器把它登记为某个客户端的出站终点；
 *   - `clientSide()` 是 `ServerGateway`：客户端用它发命令（**不带身份**）；
 *   - `LocalLink` 持有该客户端的 `client_id_t`，投递到服务器时补上 `ClientSession`；
 *   - 端点只入队，`pump()` 统一投递，避免回调内同步重入。
 */
class LocalLink {
public:
    explicit LocalLink(client_id_t client_id = 1) : client_id_(client_id) {}
    LocalLink(const LocalLink&) = delete;
    LocalLink& operator=(const LocalLink&) = delete;

    [[nodiscard]] ClientHandler& serverSide() { return server_side_; }
    [[nodiscard]] ServerGateway& clientSide() { return client_side_; }

    /** @brief 绑定两个本地 handler（构造完 controller 后调用）。 */
    void attach(ServerHandler& server, ClientHandler& client) {
        server_ = &server;
        client_ = &client;
    }

    /** @brief 投递两个方向的消息，直到两条队列都空。 */
    void pump(std::size_t max_rounds = 100000) {
        assert(server_ != nullptr && client_ != nullptr && "LocalLink::pump: 尚未 attach");
        if (server_ == nullptr || client_ == nullptr) return;

        for (std::size_t round = 0; round < max_rounds; ++round) {
            bool progress = false;

            while (!to_client_.empty()) {
                ClientDelivery deliver = std::move(to_client_.front());
                to_client_.pop();
                deliver(*client_);
                progress = true;
            }

            while (!to_server_.empty()) {
                ServerDelivery deliver = std::move(to_server_.front());
                to_server_.pop();
                deliver(*server_);
                progress = true;
            }

            if (!progress) return;
        }

        assert(false && "LocalLink::pump: 超过最大轮数（可能存在活锁）");
    }

    [[nodiscard]] bool idle() const { return to_client_.empty() && to_server_.empty(); }

private:
    using ClientDelivery = std::function<void(ClientHandler&)>;
    using ServerDelivery = std::function<void(ServerHandler&)>;

    /* 服务器出站终点：服务器看到“客户端”，调用后只入队。 */
    class ServerEndpoint final : public ClientHandler {
    public:
        explicit ServerEndpoint(LocalLink& link) : link_(link) {}

        void on(const GameEvent& event) override {
            link_.to_client_.push([event](ClientHandler& client) { client.on(event); });
        }

    private:
        LocalLink& link_;
    };

    /* 客户端出站终点：客户端看到“服务器”，调用后只入队。 */
    class ClientEndpoint final : public ServerGateway {
    public:
        explicit ClientEndpoint(LocalLink& link) : link_(link) {}

        void send(const GameCommand& command) override {
            ClientHandler* handler = &link_.server_side_;
            const client_id_t id = link_.client_id_;
            link_.to_server_.push([command, id, handler](ServerHandler& server) {
                server.on(command, ClientSession{id, *handler});
            });
        }

    private:
        LocalLink& link_;
    };

    ServerEndpoint server_side_{*this};
    ClientEndpoint client_side_{*this};
    std::queue<ClientDelivery> to_client_;
    std::queue<ServerDelivery> to_server_;
    ServerHandler* server_ = nullptr;
    ClientHandler* client_ = nullptr;
    client_id_t client_id_;
};
} // namespace poker
