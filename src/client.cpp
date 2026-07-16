#include <iostream>
#include <thread>

#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

#include "ClientNetwork.h"

const sf::IpAddress host = sf::IpAddress::LocalHost;
constexpr unsigned short port = 8088;

int main() {
    using namespace poker;
    network::ClientNetwork network;

    std::cout << "[Client] 正在尝试连接服务端...\n";

    if (network.connect(host, port)) {
        std::cout << "[Client] 成功连接到服务端！\n";
    } else {
        std::cerr << "[Client] 连接失败！请检查服务端是否已启动。\n";
        return -1;
    }

    bool running = true;
    network.setDisconnectionHandler([&running]() { running = false; });

    sf::Packet packet1;
    packet1 << "Hello";
    network.sendPacket(packet1);
    sf::Packet packet2;
    packet2 << "World!";
    network.sendPacket(packet2);
    sf::Packet packet3;
    packet3 << "finish";
    network.sendPacket(packet3);

    return 0;
}