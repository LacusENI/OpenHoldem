#include <SFML/Network.hpp>
#include <iostream>
#include <string>

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

    // 2. 发送一段纯文本字符串
    std::string textToSend = "Hello SFML Network!";
    sf::Packet sendPacket;
    sendPacket << textToSend;

    std::cout << "[Client] 正在发送消息: \"" << textToSend << "\"\n";
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