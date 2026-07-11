#include <SFML/Network.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "Card.h"

namespace poker {
NLOHMANN_JSON_SERIALIZE_ENUM(Suit, {{Suit::HEART, "Heart"},
                                    {Suit::DIAMOND, "Diamond"},
                                    {Suit::CLUB, "Club"},
                                    {Suit::SPADE, "Spade"}});

NLOHMANN_JSON_SERIALIZE_ENUM(Rank, {{Rank::TWO, "Two"},
                                    {Rank::THREE, "Three"},
                                    {Rank::FOUR, "Four"},
                                    {Rank::FIVE, "Five"},
                                    {Rank::SIX, "Six"},
                                    {Rank::SEVEN, "Seven"},
                                    {Rank::EIGHT, "Eight"},
                                    {Rank::NINE, "Nine"},
                                    {Rank::TEN, "Ten"},
                                    {Rank::JACK, "Jack"},
                                    {Rank::QUEEN, "Queen"},
                                    {Rank::KING, "King"},
                                    {Rank::ACE, "Ace"}});

void to_json(nlohmann::json& j, const Card& card) {
    j = to_string(card);
}

void from_json(const nlohmann::json& j, Card& card) {
    if (j.is_string()) {
        card = Card(j.get<std::string>().c_str());
    } else {
        throw nlohmann::json::type_error::create(302, "Expected string for Card", &j);
    }
}
} // namespace poker

int main() {
    using json = nlohmann::json;

    sf::TcpListener listener;
    unsigned short port = 53000;

    // 1. 绑定并监听端口
    if (listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "[Server] 端口 " << port << " 监听失败！\n";
        return -1;
    }
    std::cout << "[Server] 服务端已启动，正在等待客户端连接 (端口 " << port << ")...\n";

    // 2. 等待并接受客户端连接
    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Status::Done) {
        std::cerr << "[Server] 接受连接失败！\n";
        return -1;
    }

    std::cout << "[Server] 客户端已连接！IP: " << client.getRemoteAddress()->toString() << "\n";

    // 3. 接收客户端发来的数据包
    sf::Packet receivePacket;
    if (client.receive(receivePacket) == sf::Socket::Status::Done) {
        std::string jsonString;
        receivePacket >> jsonString; // 提取 JSON 文本
        json j = json::parse(jsonString);
        poker::Card card = j.get<poker::Card>();
        std::cout << "[Server] 收到客户端消息: \"" << jsonString << "\"\n";
        std::cout << "[Server] 解析出的牌: " << poker::to_string(card) << "\n";
    }

    // 4. 给客户端回复一条消息
    sf::Packet sendPacket;
    sendPacket << std::string("Hello Client, I received your message!");
    if (client.send(sendPacket) == sf::Socket::Status::Done) {
        std::cout << "[Server] 已向客户端发送回复！\n";
    }

    return 0;
}