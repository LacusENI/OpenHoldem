#include "ServerNetwork.h"

#include <spdlog/spdlog.h>

#include "Event.hpp"
#include "EventBus.hpp"

namespace poker::network {
std::string packet_to_string(sf::Packet& packet) {
    std::string msg;
    packet >> msg;
    return msg;
}
sf::Packet string_to_packet(const std::string& msg) {
    sf::Packet packet;
    packet << msg;
    return packet;
}

bool ServerNetwork::start(const unsigned short port) {
    if (listener.listen(port) != sf::Socket::Status::Done) return false;
    listener.setBlocking(false);
    return true;
}
void ServerNetwork::pull() {
    {
        // 处理有新的客户端连接加入
        auto new_client = std::make_unique<sf::TcpSocket>();
        if (listener.accept(*new_client) == sf::Socket::Status::Done) {
            new_client->setBlocking(false);
            clients.push_back(std::move(new_client));
            outgoing_packets.emplace_back();

            const int client_id = static_cast<int>(clients.size() - 1);
            event_bus->publish(NetworkEvent::ClientConnected{client_id});
        }

        // 收集所有客户端发来的消息
        for (int client_id = 0; client_id < clients.size(); client_id++) {
            const auto& client = clients[client_id];
            sf::Packet packet;
            sf::Socket::Status status = client->receive(packet);
            if (status == sf::Socket::Status::Done) {
                incoming_packets.push(packet);
                spdlog::debug("[Network] Packet Received : {}", packet_to_string(packet));
            }
            else if (status == sf::Socket::Status::Disconnected)
                event_bus->publish(NetworkEvent::ClientDisconnected{client_id});
            else if (status == sf::Socket::Status::Error) {
                spdlog::error("[Network] Error receiving packet from client {}", client_id);
            }
        }
    }
}
void ServerNetwork::push() {
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
void ServerNetwork::sendMessageToAll(std::string& msg) {
    for (auto& packet_queue : outgoing_packets) {
        packet_queue.push(string_to_packet(msg));
    }
}
void ServerNetwork::sendMessageTo(int client_id, std::string& msg) {
    auto& packet_queue = outgoing_packets.at(client_id);
    packet_queue.push(string_to_packet(msg));
}
std::string ServerNetwork::popMessage() {
    auto packet = incoming_packets.front();
    incoming_packets.pop();
    return packet_to_string(packet);
}

} // namespace poker::network