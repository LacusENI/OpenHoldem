#pragma once

#include <memory>
#include <vector>

#include "Card.h"
#include "GameTypes.h"

namespace holdem {

class IDeck;
class PlayerSet;
class BettingRound;
class PotManager;

/**
 * @brief 一局游戏的数据模型
 */
class GameModel {
public:
    GameState game_state = GameState::IDLE; // 游戏状态机

    Position btn_position = 0; // 庄家位置
    Stack big_blind = 10;      // 大盲注额

    Cards5 community_cards;
    std::unique_ptr<IDeck> deck;
    std::shared_ptr<PlayerSet> players;
    std::shared_ptr<PotManager> pot_manager;
    std::shared_ptr<BettingRound> betting_round;

    GameModel(
        std::unique_ptr<IDeck> deck,
        std::shared_ptr<PlayerSet> players,
        std::shared_ptr<PotManager> pot_manager,
        std::unique_ptr<BettingRound>);
    ~GameModel();

    /* 小盲注位 */
    Position getSmallBlindPosition() const;
    /* 大盲注位 */
    Position getBigBlindPosition() const;

    /**
     * @brief 将玩家的筹码投入底池中，并更新玩家下注额和本轮下注额
     * @note demo_v3 阶段暂不检查玩家是否有足够的筹码
     * @param position 玩家位置
     * @param amount 投入筹码的数额
     */
    void commitChips(Position position, Stack amount);

    /**
     * @brief 游戏状态和数据初始化
     */
    void setup();
    /**
     * @brief 负责发放底牌或公共牌
     */
    void dealCards();
    /**
     * @brief 进入下一个阶段，翻开此轮的公共牌
     * @note 每次发公共牌前不烧牌
     */
    void nextStreet();
    /**
     * @brief 当前玩家采取行动
     * @note demo_v2 阶段只有"投入大盲注等额"一种行动
     * @return 此次玩家所采取的动作信息
     */
    Action takeAction(const Action& action);
    /**
     * @brief 移动至下一个要行动的玩家,
     * 若下注轮已结束，则设置回合结束标志
     */
    void nextActor();

    void distributePot(const std::vector<Stack>& amounts, const std::vector<Position>& winners);

    /**
     * @brief 执行大盲注
     * @return 动作信息
     */
    Action bigBlind();
    /**
     * @brief 执行小盲注
     * @return 动作信息
     */
    Action smallBlind();
};
}
