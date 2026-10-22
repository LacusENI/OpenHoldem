#pragma once

#include "Event.hpp"
#include "ClientHandler.h"

namespace poker {
class ClientModel;
class ClientView;
class ServerGateway;

/**
 * @brief 客户端控制器
 * @details 实现 ClientHandler（接收服务器事件），并通过注入的 ServerGateway 发送命令。
 *          只做“服务器事件 → ClientModel”的投影与动作策略，不承载传输。
 */
class ClientController : public ClientHandler {
public:
    ClientController(ServerGateway& server_gateway, ClientModel& model, ClientView& view)
        : server_gateway(server_gateway), model(model), view(view) {}
    ~ClientController() override = default;

    void on(const GameEvent& event) override;

private:
    void handle(const Event::JoinResponse& event);
    void handle(const Event::PlayerJoined& event);
    void handle(const Event::GameStarted& event);
    void handle(const Event::HoleCardsDealt& event);
    void handle(const Event::StageChanged& event);
    void handle(const Event::BlindPosted& event);
    void handle(const Event::CommunityCardsDealt& event);
    void handle(const Event::TurnStarted& event);
    void handle(const Event::PlayerActed& event);
    void handle(const Event::HoleCardsRevealed& event);
    void handle(const Event::PotAwarded& event);
    void handle(const Event::UncalledBetReturned& event);
    void handle(const Event::GameEnded& event);

    ServerGateway& server_gateway;
    ClientModel& model;
    ClientView& view;};
} // namespace poker
