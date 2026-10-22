#pragma once

#include "ClientHub.h"
#include "ServerHandler.h"

namespace poker {
class GameModel;

/**
 * @brief 服务器游戏控制器（状态机）
 * @details 实现 ServerHandler（接收命令 + ClientSession），通过 ClientGateway / ClientHub 出站。
 *          阶段迁移：下注轮结束时由纯决策函数决定去向，再统一经 enterStage 进入。
 */
class ServerController : public ServerHandler {
public:
    ServerController(ClientHub& client_hub, GameModel& model)
        : client_hub(client_hub), game_model(model) {}
    ~ServerController() override = default;

    void on(const GameCommand& command, const ClientSession& sender) override;

    /* 开始下一手牌（重置状态并轮转按钮） */
    void startNextHand();

    /* 结束本下注轮：退还未跟注筹码、重置下注状态 */
    void finishBettingRound();

private:
    /* 下注轮结束后的去向（纯决策结果） */
    enum class AfterRound { HandOver, UncontestedSettle, RunoutShowdown, NextStreet };

    /* 纯决策：只读模型状态，不产生副作用 */
    AfterRound decideAfterBettingRound() const;

    /* 进入某阶段的唯一入口（设置 stage 并派发副作用） */
    void enterStage(Stage stage);
    void enterPreflop();
    void enterPostflop();
    void enterShowdown();
    void enterSettlement();
    void enterUncontestedSettle();

    /* 命令处理（由 on() 经 std::visit 分发） */
    void handle(const Command::JoinRequest& command, const ClientSession& sender);
    void handle(const Command::StartGame& command, const ClientSession& sender);
    void handle(const Command::SubmitAction& command, const ClientSession& sender);

    ClientHub& client_hub;
    GameModel& game_model;
};
} // namespace poker
