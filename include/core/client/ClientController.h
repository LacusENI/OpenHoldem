#pragma once

#include "IClient.h"

namespace poker {
class ClientModel;
class ClientView;
class IServer;

/**
 * @brief 客户端控制器
 * @details 实现 IClient（接收服务器事件），并通过注入的 IServer 发送命令。
 *          只做“服务器事件 → ClientModel”的投影与动作策略，不承载传输。
 */
class ClientController : public IClient {
public:
    ClientController(IServer& server, ClientModel& model, ClientView& view)
        : server(server), model(model), view(view) {}
    ~ClientController() override = default;

    void onJoinResponse(const Event::JoinResponse& event) override;
    void onPlayerJoined(const Event::PlayerJoined& event) override;
    void onGameStarted(const Event::GameStarted& event) override;
    void onHoleCardsDealt(const Event::HoleCardsDealt& event) override;
    void onStageChanged(const Event::StageChanged& event) override;
    void onBlindPosted(const Event::BlindPosted& event) override;
    void onCommunityCardsDealt(const Event::CommunityCardsDealt& event) override;
    void onTurnStarted(const Event::TurnStarted& event) override;
    void onPlayerActed(const Event::PlayerActed& event) override;
    void onHoleCardsRevealed(const Event::HoleCardsRevealed& event) override;
    void onPotAwarded(const Event::PotAwarded& event) override;
    void onUncalledBetReturned(const Event::UncalledBetReturned& event) override;
    void onGameEnded(const Event::GameEnded& event) override;

private:
    IServer& server;
    ClientModel& model;
    ClientView& view;
};
} // namespace poker
