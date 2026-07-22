#pragma once

#include <queue>

#include <SFML/Network.hpp>

namespace poker {
class EventBus;
} // namespace poker

namespace poker::network {
class ServerNetwork {
private:
    EventBus* event_bus;
    sf::TcpListener listener;
    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    std::queue<sf::Packet> incoming_packets;
    std::vector<std::queue<sf::Packet>> outgoing_packets;

public:
    explicit ServerNetwork(EventBus* event_bus) : event_bus(event_bus) {}
    /* 开始监听端口 */
    bool start(const unsigned short port);
    /* 拉取客户端发来的消息或处理新客户端连接加入 */
    void pull();
    /* 从缓冲队列中向客户端发送消息 */
    void push();
    /* 接收缓冲队列中是否有消息 */
    [[nodiscard]] bool hasReceivedMessage() const { return !incoming_packets.empty(); }
    /* 向所有客户端发送消息 (加入发送缓冲队列) */
    void sendMessageToAll(std::string& msg);
    /* 向指定客户端发送消息 (加入发送缓冲队列) */
    void sendMessageTo(int client_id, std::string& msg);
    /* 从接收缓冲队列中取出消息 */
    std::string popMessage();
};
} // namespace poker::network