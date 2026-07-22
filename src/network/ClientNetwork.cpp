#include "ClientNetwork.h"

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

bool ClientNetwork::connect(const sf::IpAddress& host, unsigned short port) {
    bool status =
        socket->connect(host, port, sf::seconds(TIMEOUT_SECONDS)) == sf::Socket::Status::Done;
    socket->setBlocking(false);
    return status;
}
bool ClientNetwork::hasReceivedMessage() const {
    return !incoming_packets.empty();
}
void ClientNetwork::sendMessage(const std::string& msg) {
    outgoing_packets.push(string_to_packet(msg));
}
std::string ClientNetwork::popMessage() {
    auto packet = incoming_packets.front();
    incoming_packets.pop();
    return packet_to_string(packet);
}
void ClientNetwork::pull() {
    sf::Packet packet;
    while (socket->receive(packet) == sf::Socket::Status::Done) {
        std::string message;
        sf::Packet(packet) >> message;
        spdlog::debug("[Network] Packet Received : {}", message);
        incoming_packets.push(packet);
    }
}
void ClientNetwork::push() {
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

} // namespace poker::network