#pragma once

#include "Event.hpp"

namespace poker {
/**
 * @brief 服务器 → 客户端的事件接收接口
 * @details 每个出站事件一个方法；由 ClientController 实现，
 *          LocalLink（本地回环）/ RemoteClient（网络）负责投递。
 */
class IClient {
public:
    virtual ~IClient() = default;

    virtual void onJoinResponse(const Event::JoinResponse&) = 0;
    virtual void onPlayerJoined(const Event::PlayerJoined&) = 0;
    virtual void onGameStarted(const Event::GameStarted&) = 0;
    virtual void onHoleCardsDealt(const Event::HoleCardsDealt&) = 0;
    virtual void onStageChanged(const Event::StageChanged&) = 0;
    virtual void onBlindPosted(const Event::BlindPosted&) = 0;
    virtual void onCommunityCardsDealt(const Event::CommunityCardsDealt&) = 0;
    virtual void onTurnStarted(const Event::TurnStarted&) = 0;
    virtual void onPlayerActed(const Event::PlayerActed&) = 0;
    virtual void onHoleCardsRevealed(const Event::HoleCardsRevealed&) = 0;
    virtual void onPotAwarded(const Event::PotAwarded&) = 0;
    virtual void onUncalledBetReturned(const Event::UncalledBetReturned&) = 0;
    virtual void onGameEnded(const Event::GameEnded&) = 0;
};

/** @brief 空实现：机器人 / 测试 / 占位。 */
class NullClient : public IClient {
public:
    void onJoinResponse(const Event::JoinResponse&) override {}
    void onPlayerJoined(const Event::PlayerJoined&) override {}
    void onGameStarted(const Event::GameStarted&) override {}
    void onHoleCardsDealt(const Event::HoleCardsDealt&) override {}
    void onStageChanged(const Event::StageChanged&) override {}
    void onBlindPosted(const Event::BlindPosted&) override {}
    void onCommunityCardsDealt(const Event::CommunityCardsDealt&) override {}
    void onTurnStarted(const Event::TurnStarted&) override {}
    void onPlayerActed(const Event::PlayerActed&) override {}
    void onHoleCardsRevealed(const Event::HoleCardsRevealed&) override {}
    void onPotAwarded(const Event::PotAwarded&) override {}
    void onUncalledBetReturned(const Event::UncalledBetReturned&) override {}
    void onGameEnded(const Event::GameEnded&) override {}
};
} // namespace poker
