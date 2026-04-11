#ifndef OPENHOLDEM_GAME_STATE_H
#define OPENHOLDEM_GAME_STATE_H
#include <iostream>
#include <memory>
#include "Deck.h"
#include "HandEvaluator.h"

namespace holdem {

using PlayerId = int; // 玩家ID
using Position = int; // 玩家座位
using Stack = int;    // 筹码数量

enum class GameState {
    IDLE, PREFLOP, FLOP, TURN, RIVER, AWARD
};

/**
 * @brief 玩家实体类
 */
struct Player {
    PlayerId id;       // 玩家ID
    Position position; // 玩家所在位置
    Cards2 hole_cards; // 玩家的底牌

    Stack chips = 0;       // 玩家拥有的筹码
    Stack current_bet = 0; // 玩家当前下注额

    HandValue hand_value; // 手牌分值
};

/**
 * @brief 一局游戏的数据模型
 */
class GameModel {
public:
    GameState game_state = GameState::IDLE; // 游戏状态机

    bool print_enabled = false;
    Position button_position = 0; // 庄家位置
    Stack big_blind = 10;         // 大盲注额
    Stack pot = 0;                // 底池金额
    Stack round_bet = 0;          // 此轮目前的最大下注额
    Position rest_position{};     // 休止符玩家位置(若该玩家行动后下一位是休止符玩家，本轮下注结束)
    Position current_position{};  // 当前行动玩家的位置
    bool is_round_ended = false;  // 下注轮是否已结束

    std::unique_ptr<IDeck> deck; // 牌组
    Cards5 community_cards;      // 公共牌
    std::vector<Player> players; // 玩家列表

    std::vector<Position> winners{}; // 赢家列表

    explicit GameModel(std::unique_ptr<IDeck> deck) : deck(std::move(deck)) {}

    /**
     * @brief 添加新玩家到玩家列表
     */
    void addPlayer(PlayerId id);
    /**
     * 获取到该座位上的玩家信息
     * @param position 玩家座位(从0开始)
     * @return 玩家
     */
    Player& getPlayer(Position position) {
        return players[position];
    }
    /**
     * 当前玩家的下一位(左手第一位)
     * @param position 当前玩家位置
     * @return 下一位玩家的位置
     */
    Position nextPosition(Position position) const {
        return (position + 1) % static_cast<int>(players.size());
    }
    /* 小盲注位 */
    Position getSmallBlindPosition() const {
        return nextPosition(button_position);
    }
    /* 大盲注位 */
    Position getBigBlindPosition() const {
        return nextPosition(getSmallBlindPosition());
    }
    /* 枪口位 */
    Position getUtgPosition() const {
        return nextPosition(getBigBlindPosition());
    }

    /**
     * @brief 将玩家的筹码投入底池中，并更新玩家下注额和本轮下注额
     * @note demo_v3 阶段暂不检查玩家是否有足够的筹码
     * @param position 玩家位置
     * @param amount 投入筹码的数额
     */
    void commitChips(Position position, Stack amount);

    /**
     * @brief 负责发放底牌或公共牌
     */
    void dealCards();

    /**
     * @brief 运行一局游戏
     * @details 调用此函数后，将进行一局游戏，直到结束
     */
    void run();
    /**
     * @brief 进行Preflop下注轮前的准备工作
     * @details 包括洗牌，发底牌，盲注
     */
    void preflop();
    /**
     * @brief 进入下一个阶段，翻开此轮的公共牌
     * @note 每次发公共牌前不烧牌
     */
    void nextStreet();
    /**
     * @brief 渲染控制台界面
     */
    void displayBoard();
    /**
     * @brief 进入摊牌阶段
     * @details 评估每个玩家的牌型，并决出胜者
     */
    void showdown();
    /**
     * @brief 进行一次下注轮(玩家行动)
     */
    void runBettingRound();
    /**
     * @brief 当前玩家采取行动
     * @note demo_v2 阶段只有"投入大盲注等额"一种行动
     */
    void takeAction();
    /**
     * @brief 移动至下一个要行动的玩家,
     * 若下注轮已结束，则设置回合结束标志
     */
    void nextPlayer();
    /**
     * @brief 为赢家分配底池
     * @note 若有多名玩家获胜且底池无法均分,
     * 则座位靠前的赢家比靠后的赢家多分得一个筹码
     */
    void award();
    /**
     * 控制台输出
     * @param msg 输出信息
     */
    void output(const std::string& msg) const {
        if (print_enabled) std::cout << msg;
    }
};

}

#endif //OPENHOLDEM_GAME_STATE_H