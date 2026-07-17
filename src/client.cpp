#include <iostream>
#include <chrono>
#include <thread>

#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

#include "ClientNetwork.h"
#include "EventSerializer.hpp"

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

    // 1. 发送公共牌事件
    sf::Packet packet1;
    GameEvent event1 = Event::CommunityCardsDealt{
        .community_cards = {Card("CT"), Card("DA"), Card("SQ"), Card("H5"), Card("DJ")}};
    packet1 << serializeEvent(event1).dump();
    network.sendPacket(packet1);
    std::cout << "[Client] 已投递 Event1 (CommunityCardsDealt) 到发送队列\n";

    // 2. 发送玩家下注行动事件
    sf::Packet packet2;
    GameEvent event2 = Event::PlayerActed{
        .actor_seat = 1, .action = Action::BET, .pot = 30, .player_chips = 70, .player_bet = 10};
    packet2 << serializeEvent(event2).dump();
    network.sendPacket(packet2);
    std::cout << "[Client] 已投递 Event2 (PlayerActed) 到发送队列\n";

    // 3. 发送结束测试标志
    sf::Packet packet3;
    packet3 << "finish";
    network.sendPacket(packet3);
    std::cout << "[Client] 已投递 finish 结束标志\n";

    // 4. 让网络层自转起来，把队列里的包彻底冲刷到网卡中
    std::cout << "[Client] 正在冲刷网络缓冲区并等待服务端处理...\n";

    auto start_time = std::chrono::steady_clock::now();
    bool running = true;

    while (running) {
        network.poll();

        // 给网络层 3 秒钟的时间发送，发送完毕后客户端自动退出
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() >= 3) {
            std::cout << "[Client] 测试数据发送完毕，主动退出。\n";
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}