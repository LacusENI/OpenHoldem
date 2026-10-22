#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <queue>
#include <type_traits>
#include <utility>

#include "IClient.h"
#include "IServer.h"

namespace poker {
/**
 * @brief 本地回环：异步、双向、FIFO
 * @details 两侧各暴露一个端点：`serverSide()` 是 IClient（服务器出站），
 *          `clientSide()` 是 IServer（客户端出站）。
 *          端点上的 onXXX 只入队，`pump()` 再统一投递，避免回调内同步重入。
 *          语义与网络一致：发送立即返回，接收方在下一次 pump 处理。
 */
class LocalLink {
public:
    LocalLink() = default;
    LocalLink(const LocalLink&) = delete;
    LocalLink& operator=(const LocalLink&) = delete;

    [[nodiscard]] IClient& serverSide() { return server_side_; }
    [[nodiscard]] IServer& clientSide() { return client_side_; }

    /** @brief 绑定两个本地 handler（构造完 controller 后调用）。 */
    void attach(IServer& server, IClient& client) {
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
    using ClientDelivery = std::function<void(IClient&)>;
    using ServerDelivery = std::function<void(IServer&)>;

    /* 服务器出站端点：服务器看到“客户端”，调用后只入队。 */
    class ServerEndpoint final : public IClient {
    public:
        explicit ServerEndpoint(LocalLink& link) : link_(link) {}

        void onJoinResponse(const Event::JoinResponse& e) override { enqueue(e); }
        void onPlayerJoined(const Event::PlayerJoined& e) override { enqueue(e); }
        void onGameStarted(const Event::GameStarted& e) override { enqueue(e); }
        void onHoleCardsDealt(const Event::HoleCardsDealt& e) override { enqueue(e); }
        void onStageChanged(const Event::StageChanged& e) override { enqueue(e); }
        void onBlindPosted(const Event::BlindPosted& e) override { enqueue(e); }
        void onCommunityCardsDealt(const Event::CommunityCardsDealt& e) override { enqueue(e); }
        void onTurnStarted(const Event::TurnStarted& e) override { enqueue(e); }
        void onPlayerActed(const Event::PlayerActed& e) override { enqueue(e); }
        void onHoleCardsRevealed(const Event::HoleCardsRevealed& e) override { enqueue(e); }
        void onPotAwarded(const Event::PotAwarded& e) override { enqueue(e); }
        void onUncalledBetReturned(const Event::UncalledBetReturned& e) override { enqueue(e); }
        void onGameEnded(const Event::GameEnded& e) override { enqueue(e); }

    private:
        template <typename TEvent>
        void enqueue(const TEvent& event) {
            link_.to_client_.push([event](IClient& client) { deliver(client, event); });
        }

        template <typename TEvent>
        static void deliver(IClient& client, const TEvent& event) {
            if constexpr (std::is_same_v<TEvent, Event::JoinResponse>) {
                client.onJoinResponse(event);
            } else if constexpr (std::is_same_v<TEvent, Event::PlayerJoined>) {
                client.onPlayerJoined(event);
            } else if constexpr (std::is_same_v<TEvent, Event::GameStarted>) {
                client.onGameStarted(event);
            } else if constexpr (std::is_same_v<TEvent, Event::HoleCardsDealt>) {
                client.onHoleCardsDealt(event);
            } else if constexpr (std::is_same_v<TEvent, Event::StageChanged>) {
                client.onStageChanged(event);
            } else if constexpr (std::is_same_v<TEvent, Event::BlindPosted>) {
                client.onBlindPosted(event);
            } else if constexpr (std::is_same_v<TEvent, Event::CommunityCardsDealt>) {
                client.onCommunityCardsDealt(event);
            } else if constexpr (std::is_same_v<TEvent, Event::TurnStarted>) {
                client.onTurnStarted(event);
            } else if constexpr (std::is_same_v<TEvent, Event::PlayerActed>) {
                client.onPlayerActed(event);
            } else if constexpr (std::is_same_v<TEvent, Event::HoleCardsRevealed>) {
                client.onHoleCardsRevealed(event);
            } else if constexpr (std::is_same_v<TEvent, Event::PotAwarded>) {
                client.onPotAwarded(event);
            } else if constexpr (std::is_same_v<TEvent, Event::UncalledBetReturned>) {
                client.onUncalledBetReturned(event);
            } else if constexpr (std::is_same_v<TEvent, Event::GameEnded>) {
                client.onGameEnded(event);
            } else {
                static_assert(sizeof(TEvent) == 0, "LocalLink: 未处理的事件类型");
            }
        }

        LocalLink& link_;
    };

    /* 客户端出站端点：客户端看到“服务器”，调用后只入队。 */
    class ClientEndpoint final : public IServer {
    public:
        explicit ClientEndpoint(LocalLink& link) : link_(link) {}

        void onJoin(IClient& client, const Command::JoinRequest& command) override {
            IClient* client_ptr = &client;
            link_.to_server_.push([client_ptr, command](IServer& server) {
                server.onJoin(*client_ptr, command);
            });
        }

        void onStart(const Command::StartGame& command) override {
            link_.to_server_.push(
                [command](IServer& server) { server.onStart(command); });
        }

        void onSubmit(const Command::SubmitAction& command) override {
            link_.to_server_.push(
                [command](IServer& server) { server.onSubmit(command); });
        }

    private:
        LocalLink& link_;
    };

    ServerEndpoint server_side_{*this};
    ClientEndpoint client_side_{*this};
    std::queue<ClientDelivery> to_client_;
    std::queue<ServerDelivery> to_server_;
    IServer* server_ = nullptr;
    IClient* client_ = nullptr;
};
} // namespace poker
