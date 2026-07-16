#pragma once
#include <functional>
#include <queue>

#include <SFML/Network.hpp>

constexpr float TIMEOUT_SECONDS = 5;

namespace poker::network {
class ClientNetwork {
private:
    std::queue<sf::Packet> incoming_packets;
    std::queue<sf::Packet> outgoing_packets;
    std::unique_ptr<sf::TcpSocket> socket;
    std::function<void()> disconnection_handler;

public:
    ClientNetwork() : socket(std::make_unique<sf::TcpSocket>()) {}
    ~ClientNetwork() = default;

    bool connect(const sf::IpAddress& host, unsigned short port) {
        bool status = socket->connect(host, port, sf::seconds(TIMEOUT_SECONDS)) ==
               sf::Socket::Status::Done;
        socket->setBlocking(false);
        return status;
    }
    bool hasPackets() const { return !incoming_packets.empty(); }
    void sendPacket(const sf::Packet& packet) { outgoing_packets.push(packet); }
    sf::Packet popPacket() {
        auto packet = incoming_packets.front();
        incoming_packets.pop();
        return packet;
    }
    void poll() {
        sf::Packet packet;
        while (socket->receive(packet) == sf::Socket::Status::Done) {
            incoming_packets.push(packet);
        }
        while (!outgoing_packets.empty()) {
            packet = outgoing_packets.front();
            sf::Socket::Status status = socket->send(packet);
            if (status == sf::Socket::Status::Done) {
                outgoing_packets.pop();
                continue;
            }
            if (status == sf::Socket::Status::Disconnected) {
                disconnection_handler();
            }
            break;
        }
    }
    void setDisconnectionHandler(const std::function<void()>& handler) {
        disconnection_handler = handler;
    }
};
} // namespace poker::network