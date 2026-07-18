#pragma once

#include <queue>

#include <SFML/Network.hpp>

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
    bool start(const unsigned short port) {
        if (listener.listen(port) != sf::Socket::Status::Done) return false;
        listener.setBlocking(false);
        return true;
    }
    void poll() {
        // 处理有新的客户端连接加入
        auto new_client = std::make_unique<sf::TcpSocket>();
        if (listener.accept(*new_client) == sf::Socket::Status::Done) {
            new_client->setBlocking(false);
            clients.push_back(std::move(new_client));
            outgoing_packets.push_back(std::queue<sf::Packet>());

            const int client_id = static_cast<int>(clients.size() - 1);
            event_bus->publish(NetworkEvent::ClientConnected{client_id});
        }

        // 收集所有客户端发来的消息
        for (int client_id = 0; client_id < clients.size(); client_id++) {
            const auto& client = clients[client_id];
            sf::Packet packet;
            sf::Socket::Status status = client->receive(packet);
            if (status == sf::Socket::Status::Done) {
                std::string message;
                sf::Packet(packet) >> message;
                spdlog::debug("[Network] Packet Received : {}", message);
                incoming_packets.push(std::move(packet));
            }
            if (status == sf::Socket::Status::Disconnected)
                event_bus->publish(NetworkEvent::ClientDisconnected{client_id});
            else if (status == sf::Socket::Status::Error) {
                spdlog::error("[Network] Error receiving packet from client {}", client_id);
            }
        }
    }
    void push() {
        for (int client_id = 0; client_id < clients.size(); client_id++) {
            const auto& client = clients[client_id];
            auto& packet_queue = outgoing_packets[client_id];
            while (!packet_queue.empty()) {
                sf::Packet& packet = packet_queue.front();
                std::string message;
                sf::Packet(packet) >> message;
                sf::Socket::Status status = client->send(packet);
                if (status == sf::Socket::Status::Done) {
                    packet_queue.pop();
                    spdlog::debug("[Network] Packet Sent Successfully : {}", message);
                    continue;
                }
                if (status == sf::Socket::Status::Disconnected)
                    event_bus->publish(NetworkEvent::ClientDisconnected{client_id});
                break;
            }
        }
    }
    bool hasPackets() const { return !incoming_packets.empty(); }
    void sendPacketToAll(sf::Packet& packet) {
        for (auto& packet_queue : outgoing_packets) {
            packet_queue.push(packet);
        }
    }
    void sendPacketTo(int client_id, sf::Packet& packet) {
        auto& packet_queue = outgoing_packets.at(client_id);
        packet_queue.push(packet);
    }
    sf::Packet popPacket() {
        auto packet = incoming_packets.front();
        incoming_packets.pop();
        return packet;
    }
};
} // namespace poker::network