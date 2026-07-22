#include <thread>

#include <SFML/Network.hpp>

#include "BettingRound.h"
#include "Deck.hpp"
#include "Event.hpp"
#include "EventBus.hpp"
#include "EventLogger.hpp"
#include "EventSerializer.h"
#include "GameController.h"
#include "GameModel.h"
#include "PotManager.h"
#include "SeatManager.h"
#include "ServerIoAdapter.h"
#include "ServerNetwork.h"

constexpr unsigned short port = 8088;

int main() {
    using namespace poker;

    spdlog::set_level(spdlog::level::debug);

    EventBus event_bus;
    Deck deck;
    BettingRound betting_round;
    PotManager pot_manager;
    SeatManager seat_manager;
    GameModel game_model(&event_bus, &deck, &betting_round, &pot_manager, &seat_manager);
    GameController game_controller(&event_bus, &game_model);
    ServerIoAdapter io_adapter(&event_bus);

    network::ServerNetwork network(&event_bus);
    logging::EventLogger event_logger(&event_bus, "Server");

    event_logger.subscribeEvent();
    io_adapter.subscribeEvent();
    game_controller.subscribeEvent();

    GlobalLogger::trace = [](std::string_view message) { spdlog::trace(message); };
    GlobalLogger::info = [](std::string_view message) { spdlog::info(message); };
    GlobalLogger::debug = [](std::string_view message) { spdlog::debug(message); };
    GlobalLogger::warn = [](std::string_view message) { spdlog::warn(message); };
    GlobalLogger::error = [](std::string_view message) { spdlog::error(message); };
    GlobalLogger::critical = [](std::string_view message) { spdlog::critical(message); };

    io_adapter.sendToAllClient = [&network](const GameEvent& event) {
        std::string message = serializeEvent(event);
        network.sendMessageToAll(message);
    };
    io_adapter.sendToOneClient = [&network](int client_id, const GameEvent& event) {
        std::string message = serializeEvent(event);
        network.sendMessageTo(client_id, message);
    };

    bool connected = false;
    for (int i = 0; i < 5; i++) {
        if (network.start(port)) {
            spdlog::info(std::format("[Server] 服务端已启动，正在等待客户端连接 (端口 {})...", port));
            connected = true;
            break;
        }
        spdlog::error(std::format("[Server] 端口 {} 监听失败！", port));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    if (!connected) {
        spdlog::critical(std::format("[Server] 端口 {} 监听失败，服务端启动失败！", port));
        return -1;
    }

    bool running = true;

    event_bus.subscribe<Event::GameEnded>([&running](const auto& event) { running = false; });

    spdlog::info("[Server] 将在有3名玩家加入对局时开始游戏");

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

        // 推送所有需要发送给客户端的事件
        network.push();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    spdlog::info("[Server] 服务端安全退出");
    return 0;
}