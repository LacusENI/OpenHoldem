#pragma once

#include <vector>

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class EventBus;
class GameModel;

/**
 * @brief 游戏控制器
 */
class GameController {
public:
    EventBus* event_bus;
    GameModel* game_model;

    explicit GameController(EventBus* event_bus, GameModel* model);
    ~GameController() = default;

    /* 订阅事件 */
    void subscribeEvent();
    /* 处理玩家加入请求 */
    void handle(const Event::PlayerJoinRequest& event);
    /* 处理玩家输入 */
    void handle(const Event::PlayerAction& event);
    /* 开始一局游戏 */
    void handle(const Event::StartGame& event);

    /* 翻牌前阶段 */
    void processPreflopStage();
    /* 翻牌后阶段 */
    void processPostflopStage();
    /* 摊牌阶段 */
    void processShowdownStage(); 
};

} // namespace poker
