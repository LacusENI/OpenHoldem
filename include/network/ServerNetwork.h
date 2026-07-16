#pragma once

#include <queue>
#include <functional>

#include <SFML/Network.hpp>

namespace poker::network {
class ServerNetwork {
private:
    sf::TcpListener listener;
    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    std::queue<sf::Packet> incoming_packets;
    std::function<void()> disconnection_handler;

public:
    bool start(const unsigned short port) {
        return listener.listen(port) == sf::Socket::Status::Done;
    }
    void update() {
        auto new_client = std::make_unique<sf::TcpSocket>();
        if (listener.accept(*new_client) == sf::Socket::Status::Done) {
            new_client->setBlocking(false);
            clients.push_back(std::move(new_client));
        }
        for (const auto& client : clients) {
            sf::Packet packet;
            sf::Socket::Status status = client->receive(packet);
            if (status == sf::Socket::Status::Done) {
                incoming_packets.push(std::move(packet));
            }
            else if (status == sf::Socket::Status::Disconnected) {
                disconnection_handler();
            }
        }
    }
    bool hasPackets() const { return !incoming_packets.empty(); }
    void sendPacketToAll(sf::Packet& packet) {
        for (const auto& client : clients) {
            sf::Socket::Status status = client->send(packet);
            if (status == sf::Socket::Status::Disconnected) {
                disconnection_handler();
            }
        }
    }
    sf::Packet popPacket() {
        auto packet = incoming_packets.front();
        incoming_packets.pop();
        return packet;
    }
    void setDisconnectionHandler(const std::function<void()>& handler) {
        disconnection_handler = handler;
    }
};
} // namespace poker::network