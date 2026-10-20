#include <thread>

#include <SFML/Network.hpp>

#include "ClientController.h"
#include "ClientIoAdapter.h"
#include "ClientModel.h"
#include "ClientNetwork.h"
#include "ClientView.h"
#include "ConsoleView.h"
#include "EventBus.hpp"
#include "EventLogger.hpp"
#include "EventSerializer.h"

constexpr unsigned short port = 8088;

int main() {
    const sf::IpAddress host = sf::IpAddress::LocalHost;

    using namespace poker;

    spdlog::set_level(spdlog::level::debug);

    EventBus event_bus;
    ui::ConsoleView console_view;
    ClientModel client_model;
    ClientController client_controller(&event_bus, &client_model, &console_view);
    ClientIoAdapter io_adapter(&event_bus);

    network::ClientNetwork network(&event_bus);
    logging::EventLogger event_logger(&event_bus, "Client");
    spdlog::info("[Client] 正在尝试连接服务端...\n");

    event_logger.subscribeEvent();
    io_adapter.subscribeEvent();
    client_controller.subscribeEvent();

    io_adapter.sendToServer = [&network](const GameEvent& event) {
        std::string message = serializeEvent(event);
        network.sendMessage(message);
    };

    if (network.connect(host, port)) {
        spdlog::info("[Client] 成功连接到服务端！");
    } else {
        spdlog::error("[Client] 连接失败！请检查服务端是否已启动");
        return -1;
    }
    bool running = true;

    event_bus.subscribe<NetworkEvent::ServerDisconnected>([&running](const auto& event) {
        spdlog::error("[Client] 与服务端断开连接！");
        running = false;
    });
    event_bus.subscribe<Event::GameEnded>([&running](const auto& event) { running = false; });

    while (running) {
        // 网络拉取接收到的消息
        network.pull();

        // 处理消息并反序列化为事件，存入事件队列
        std::queue<GameEvent> event_queue;
        while (network.hasReceivedMessage()) {
            std::string message = network.popMessage();
            std::optional<GameEvent> event = deserializeEvent(message);
            if (event) event_queue.push(*event);
        }
        // 从事件队列依次取出并发布事件
        while (!event_queue.empty()) {
            GameEvent event = event_queue.front();
            event_queue.pop();
            std::visit([&](const auto& e) { event_bus.publish(e); }, event);
        }
        // 业务核心响应事件并输出事件

        // 推送所有需要发送给服务器的事件
        network.push();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    spdlog::info("[Client] 客户端已安全退出");
    return 0;
}