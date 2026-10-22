#pragma once

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class ClientHandler;

/**
 * @brief 一个客户端的会话：稳定 id + 回信通道（非拥有）。
 * @details 由**服务器侧接入适配器**（LocalLink / 网络监听）构造并传入，
 *          客户端自身不需要、也无法提供它。
 */
struct ClientSession {
    client_id_t id;
    ClientHandler& handler;
};

/**
 * @brief 服务器入站：接收客户端命令
 * @details `sender` 由传输层补上（不是客户端传的）：
 *          - `JoinRequest` 用它登记座位；
 *          - `SubmitAction` 可用 `sender.id` 校验"提交者 == 当前行动者"。
 */
class ServerHandler {
public:
    virtual ~ServerHandler() = default;

    virtual void on(const GameCommand& command, const ClientSession& sender) = 0;
};
} // namespace poker
