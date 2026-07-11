#include <SFML/Network.hpp>
#include <iostream>
#include <string>

int main() {
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

    std::cout << "[Server] 客户端已连接！IP: "
              << client.getRemoteAddress()->toString() << "\n";

    // 3. 接收客户端发来的数据包
    sf::Packet receivePacket;
    if (client.receive(receivePacket) == sf::Socket::Status::Done) {
        std::string message;
        receivePacket >> message; // 提取字符串

        std::cout << "[Server] 收到客户端消息: \"" << message << "\"\n";
    }

    // 4. 给客户端回复一条消息
    sf::Packet sendPacket;
    sendPacket << std::string("Hello Client, I received your message!");
    if (client.send(sendPacket) == sf::Socket::Status::Done) {
        std::cout << "[Server] 已向客户端发送回复！\n";
    }

    return 0;
}