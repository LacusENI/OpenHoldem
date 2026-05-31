#pragma once

#include <memory>
#include <vector>

#include "Card.h"
#include "GameTypes.h"

namespace poker {

class IDeck;
class SeatingManager;
class BettingRound;
class PotManager;

/**
 * @brief 一局游戏的数据模型
 */
class GameModel {
public:
    GameState game_state = GameState::IDLE; // 游戏状态机

    seat_idx_t btn_position = 0; // 庄家位置
    chips_t big_blind = 10;      // 大盲注额

    Cards5 community_cards;
    std::unique_ptr<IDeck> deck;
    std::shared_ptr<PotManager> pot_manager;
    std::shared_ptr<SeatingManager> seating_manager;
    std::shared_ptr<BettingRound> betting_round;

    GameModel(
        std::unique_ptr<IDeck> deck,
        std::shared_ptr<PotManager> pot_manager,
        std::shared_ptr<SeatingManager> players,
        std::unique_ptr<BettingRound>);
    ~GameModel();

    /**
     * @brief 游戏状态和数据初始化
     */
    void startGame();
    /**
     * @brief 负责发放底牌或公共牌
     */
    void dealCards();
    /**
     * @brief 进入下一个阶段，翻开此轮的公共牌
     * @note 每次发公共牌前不烧牌
     */
    void nextStreet();

    void distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners);

};
} // namespace poker
