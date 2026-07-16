#pragma once

#include <memory>
#include <vector>

#include "Card.hpp"
#include "Event.hpp"
#include "GameTypes.hpp"
#include "chips_calc.h"

namespace poker {
class EventBus;
class IDeck;
class SeatManager;
class BettingRound;
class PotManager;

/**
 * @brief 游戏领域模型
 */
class GameModel {
private:
    std::shared_ptr<EventBus> event_bus;
    std::unique_ptr<IDeck> deck;
    std::shared_ptr<BettingRound> betting_round;
    std::shared_ptr<PotManager> pot_manager;
    std::shared_ptr<SeatManager> seat_manager;

    Stage current_stage = Stage::IDLE; // 游戏状态机
    chips_t big_blind = 10;            // 大盲注额
    cards_5_t community_cards;

    void takePlayerChipsToPot(seat_idx_t player_seat, chips_t chips);
    void takePotChipsToPlayer(seat_idx_t player_seat, chips_t chips);
    void applyBettingResult(seat_idx_t player_seat, algo::BettingResult result);

public:
    GameModel(std::shared_ptr<EventBus> event_bus, std::unique_ptr<IDeck> deck,
              std::shared_ptr<PotManager> pot_manager, std::shared_ptr<SeatManager> seat_manager,
              std::unique_ptr<BettingRound> betting_round);
    ~GameModel();

    seat_idx_t getBtnSeat() const;
    Stage getCurrentStage() const;
    void setCurrentStage(Stage next_street);
    chips_t getBigBlind() const;
    cards_5_t getCommunityCards() const;
    seat_idx_t getActorSeat() const;
    bool isRoundFinished() const;

    /* 加入新玩家 */
    void joinNewPlayer();
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
    void setupRoundStart();
    /* 等待玩家输入 */
    void awaitPlayerInput();
    /* 移至下一个行动玩家 */
    void advanceToNextActor();
    /* 处理玩家输入 */
    void processPlayerInput(const Event::PlayerActionSubmitted& event);
    /* 处理摊牌 */
    void revealHoleCards();
    void distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners);
};
} // namespace poker
