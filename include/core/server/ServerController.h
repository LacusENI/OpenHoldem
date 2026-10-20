#pragma once

#include "ClientGateway.h"
#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class EventBus;
class GameModel;

/**
 * @brief 服务器游戏控制器（状态机）
 * @details 阶段迁移：下注轮结束时由纯决策函数决定去向，再统一经 enterStage 进入。
 */
class ServerController {
private:
    EventBus& event_bus;
    ClientGateway& client_gateway;
    GameModel& game_model;

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

public:
    ServerController(EventBus& event_bus, ClientGateway& client_gateway, GameModel& model)
        : event_bus(event_bus), client_gateway(client_gateway), game_model(model) {}
    ~ServerController() = default;

    /* 订阅事件 */
    void subscribeEvent();
    /* 处理玩家加入请求 */
    void handle(const Command::JoinRequest&);
    /* 处理玩家输入 */
    void handle(const Command::SubmitAction&);
    /* 开始一局游戏 */
    void handle(const Command::StartGame&);

    /* 开始下一手牌（重置状态并轮转按钮） */
    void startNextHand();

    /* 结束本下注轮：退还未跟注筹码、重置下注状态并发布结束事件 */
    void finishBettingRound();
};

} // namespace poker
