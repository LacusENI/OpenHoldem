#include <SFML/Network.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "Card.h"
#include "Event.h"
#include "EventBus.h"

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

namespace Event {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PotUpdated, pot);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoundBetUpdated, round_bet);
} // namespace Event
} // namespace poker

template <>
struct nlohmann::adl_serializer<poker::PokerEvent> {
    static void to_json(json& j, const poker::PokerEvent& event) {
        std::visit(
            [&j]<typename T>(const T& e) {
                if constexpr (std::is_same_v<T, poker::Event::PotUpdated>) {
                    j = json{{"type", "PotUpdated"}, {"data", e}};
                } else if constexpr (std::is_same_v<T, poker::Event::RoundBetUpdated>) {
                    j = json{{"type", "RoundBetUpdated"}, {"data", e}};
                }
            },
            event);
    }

    static void from_json(const json& j, poker::PokerEvent& event) {
        std::string type_str = j.at("type").get<std::string>();
        const auto& data_json = j.at("data");

        if (type_str == "PotUpdated") {
            event = data_json.get<poker::Event::PotUpdated>();
        } else if (type_str == "RoundBetUpdated") {
            event = data_json.get<poker::Event::RoundBetUpdated>();
        } else {
            throw std::runtime_error("Unknown PokerEvent type: " + type_str);
        }
    }
};

int main() {
    sf::TcpSocket socket;
    sf::IpAddress ip = sf::IpAddress::LocalHost;
    unsigned short port = 53000;
    poker::EventBus event_bus;

    event_bus.subscribe<poker::Event::PotUpdated>([](const poker::Event::PotUpdated& event) {
        std::cout << "[Client] 事件发布: PotUpdated | pot=" << event.pot;
    });

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
    std::cout << "[Client] 序列化后的 JSON 字符串: " << j.dump() << "\n";
    sf::Packet sendPacket;
    sendPacket << j.dump();

    std::cout << "[Client] 正在发送消息: " << j.dump() << "\n";
    if (socket.send(sendPacket) != sf::Socket::Status::Done) {
        std::cerr << "[Client] 消息发送失败！\n";
        return -1;
    }

    // 3. 接收服务端的回复
    sf::Packet receivePacket;
    if (socket.receive(receivePacket) == sf::Socket::Status::Done) {
        std::string replyMessage;
        receivePacket >> replyMessage;
        nlohmann::json json = nlohmann::json::parse(replyMessage);
        poker::PokerEvent event = json.get<poker::PokerEvent>();
        std::cout << "[Client] 收到服务端回复: \"" << replyMessage << "\"\n";
        std::visit([&](const auto& e) { event_bus.publish(e); }, event);
    }

    std::cout << "[Client] 通信测试顺利完成！\n";
    return 0;
}