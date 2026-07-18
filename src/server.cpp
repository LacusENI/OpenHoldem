#include <iostream>
#include <thread>

#include <SFML/Network.hpp>

#include "Event.hpp"
#include "EventBus.hpp"
#include "EventLogger.hpp"
#include "EventSerializer.h"
#include "ServerNetwork.h"

const sf::IpAddress host = sf::IpAddress::LocalHost;
constexpr unsigned short port = 8088;

int main() {
    using namespace poker;
    network::ServerNetwork network;

    EventBus event_bus;
    logging::EventLogger event_logger(&event_bus, "Client");

    if (network.start(port)) {
        std::cout << "[Server] 服务端已启动，正在等待客户端连接 (端口 " << port << ")...\n";
    } else {
        std::cerr << "[Server] 端口 " << port << " 监听失败！\n";
        return -1;
    }

    bool running = true;
    network.setDisconnectionHandler([&running]() {
        std::cout << "[Server] 检测到客户端网络断开。\n";
        running = false;
    });

    while (running) {
        network.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        while (network.hasPackets()) {
            std::string message;
            sf::Packet packet = network.popPacket();
            packet >> message;

            if (message == "finish") {
                std::cout << "[Server] 收到 'finish' 指令，准备关闭服务端...\n";
                running = false;
            } else {
                std::cout << "[Server] 收到原始 JSON 数据: " << message << "\n";

                std::optional<GameEvent> received_event = deserializeEvent(message);
                if (received_event == std::nullopt) {
                    std::cerr << "[Server] 反序列化失败！\n";
                } else {
                    std::cout << "[Server] 序列化/反序列化校验成功！事件解析正常。\n\n";
                }
            }
        }
    }

    std::cout << "[Server] 服务端安全退出。\n";
    return 0;
}