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
    EventBus* event_bus;
    std::queue<sf::Packet> incoming_packets;
    std::queue<sf::Packet> outgoing_packets;
    std::unique_ptr<sf::TcpSocket> socket;

public:
    ClientNetwork(EventBus* event_bus)
        : event_bus(event_bus), socket(std::make_unique<sf::TcpSocket>()) {}
    ~ClientNetwork() = default;

    bool connect(const sf::IpAddress& host, unsigned short port) {
        bool status =
            socket->connect(host, port, sf::seconds(TIMEOUT_SECONDS)) == sf::Socket::Status::Done;
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
            std::string message;
            sf::Packet(packet) >> message;
            spdlog::debug("[Network] Packet Received : {}", message);
            incoming_packets.push(packet);
        }
    }
    void push() {
        while (!outgoing_packets.empty()) {
            sf::Packet& packet = outgoing_packets.front();
            std::string message;
            sf::Packet(packet) >> message;
            sf::Socket::Status status = socket->send(packet);
            if (status == sf::Socket::Status::Done) {
                outgoing_packets.pop();
                spdlog::debug("[Network] Packet Sent Successfully : {}", message);
                continue;
            }
            if (status == sf::Socket::Status::Disconnected)
                event_bus->publish(NetworkEvent::ServerDisconnected{});
            break;
        }
    }
};
} // namespace poker::network