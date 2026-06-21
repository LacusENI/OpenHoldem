#pragma once

#include <memory>
#include <vector>

#include "Card.h"
#include "GameTypes.h"
#include "chips_calc.h"
#include "Event.h"

namespace poker {
class EventBus;
class IDeck;
class SeatingManager;
class BettingRound;
class PotManager;

/**
 * @brief 游戏领域模型
 */
class GameModel {
private:
    std::shared_ptr<EventBus> event_bus;
    std::unique_ptr<IDeck> deck;
    std::shared_ptr<PotManager> pot_manager;
    std::shared_ptr<SeatingManager> seating_manager;
    std::shared_ptr<BettingRound> betting_round;

    Street street = Street::IDLE; // 游戏状态机
    chips_t big_blind = 10;      // 大盲注额
    cards_5_t community_cards;

    void takePlayerChipsToPot(seat_idx_t player_seat, chips_t chips);
    void takePotChipsToPlayer(seat_idx_t player_seat, chips_t chips);
    void applyBettingResult(seat_idx_t player_seat, math::BettingResult result);
public:
    GameModel(
        std::shared_ptr<EventBus> event_bus,
        std::unique_ptr<IDeck> deck,
        std::shared_ptr<PotManager> pot_manager,
        std::shared_ptr<SeatingManager> players,
        std::unique_ptr<BettingRound> betting_round);
    ~GameModel();

    Street getStreet() const;
    chips_t getBigBlind() const;
    cards_5_t getCommunityCards() const;
    seat_idx_t getActorSeat() const;
    bool isRoundFinished() const;

    /* 开始一局游戏 */
    void startGame();
    /* 向所有玩家发放底牌 */
    void dealHoleCards();
    /* 发放公共牌 */
    void dealCommunityCards();
    /* 执行盲注 */
    void postForcedBlind();
    /* 进入第一个阶段(Pre-flop) */
    void enterPreflopStreet();
    /* 进入下一个阶段 */
    void advanceToNextStreet();
    /* 开始下注轮 */
    void startBettingRound();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 移至下一个行动玩家 */
    void advanceToNextActor();
    /* 处理玩家输入 */
    void processPlayerInput(const Event::PlayerInputSubmitted& event);
    /* 处理摊牌 */
    void processShowdown();
    void distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners);
};
} // namespace poker
