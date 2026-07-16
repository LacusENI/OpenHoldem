#include <ServerNetwork.h>

#include <iostream>
#include <thread>

#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

const sf::IpAddress host = sf::IpAddress::LocalHost;
constexpr unsigned short port = 8088;

int main() {
    using namespace poker;
    network::ServerNetwork network;

    if (network.start(port)) {
        std::cout << "[Server] 服务端已启动，正在等待客户端连接 (端口 " << port << ")...\n";
    } else {
        std::cerr << "[Server] 端口 " << port << " 监听失败！\n";
        return -1;
    }

    bool running = true;
    network.setDisconnectionHandler([&running]() { running = false; });
    while (running) {
        network.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        while (network.hasPackets()) {
            std::string message;
            sf::Packet packet = network.popPacket();
            packet >> message;
            if (message == "finish") {
                running = false;
            } else {
                std::cout << message << "\n";
            }
        }
    }
    return 0;
}