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
    sf::TcpSocket socket;
    sf::IpAddress ip = sf::IpAddress::LocalHost;
    unsigned short port = 53000;

    // 1. 连接本地服务端
    std::cout << "[Client] 正在尝试连接服务端...\n";

    if (socket.connect(ip, port) != sf::Socket::Status::Done) {
        std::cerr << "[Client] 连接失败！请检查服务端是否已启动。\n";
        return -1;
    }
    std::cout << "[Client] 成功连接到服务端！\n";

    // 2. 发送一张牌
    nlohmann::json j =
        poker::Card(poker::Suit::HEART, poker::Rank::ACE); // 创建一个 Card 对象并序列化为 JSON
    std::string jsonString = j.dump();
    std::cout << "[Client] 序列化后的 JSON 字符串: " << jsonString << "\n";
    sf::Packet sendPacket;
    sendPacket << jsonString;

    std::cout << "[Client] 正在发送消息: " << jsonString << "\n";
    if (socket.send(sendPacket) != sf::Socket::Status::Done) {
        std::cerr << "[Client] 消息发送失败！\n";
        return -1;
    }

    // 3. 接收服务端的回复
    sf::Packet receivePacket;
    if (socket.receive(receivePacket) == sf::Socket::Status::Done) {
        std::string replyMessage;
        receivePacket >> replyMessage;

        std::cout << "[Client] 收到服务端回复: \"" << replyMessage << "\"\n";
    }

    std::cout << "[Client] 通信测试顺利完成！\n";
    return 0;
}