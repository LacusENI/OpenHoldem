#pragma once

#include <spdlog/spdlog.h>

#include <queue>

#include <SFML/Network.hpp>

#include "Event.hpp"
#include "EventBus.hpp"

namespace poker {
class EventBus;
}
constexpr float TIMEOUT_SECONDS = 5;

namespace poker::network {
class ClientNetwork {
private:
    EventBus *event_bus;
    std::queue<sf::Packet> incoming_packets;
    std::queue<sf::Packet> outgoing_packets;
    std::unique_ptr<sf::TcpSocket> socket;

public:
    ClientNetwork(EventBus *event_bus)
        : event_bus(event_bus), socket(std::make_unique<sf::TcpSocket>()) {}
    ~ClientNetwork() = default;
    /* 建立与服务器的通信连接 */
    bool connect(const sf::IpAddress &host, unsigned short port);
    /* 接收缓冲队列中是否有消息 */
    [[nodiscard]] bool hasReceivedMessage() const;
    /* 向服务器发送消息 (加入发送缓冲队列) */
    void sendMessage(const std::string &msg);
    /* 从接收缓冲队列中取出消息 */
    std::string popMessage();
    /* 拉取服务器发来的消息 */
    void pull();
    /* 从缓冲队列中向客户端发送消息 */
    void push();
};
} // namespace poker::network
