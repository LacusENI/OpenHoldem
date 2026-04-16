#ifndef OPENHOLDEM_GAME_STATE_H
#define OPENHOLDEM_GAME_STATE_H
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

struct Action {
    Position actor_position;
    std::string action_name;
    Stack amount;
};

/**
 * @brief 玩家实体类
 */
struct Player {
    PlayerId id;       // 玩家ID
    Position position; // 玩家所在位置
    Cards2 hole_cards; // 玩家的底牌

    Stack chips = 0;        // 玩家拥有的筹码
    Stack current_bet = 0;  // 玩家当前下注额
    bool is_folded = false; // 玩家是否已弃牌

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

    /**
     * 获取当前玩家的下一个可行动的玩家位置
     * @param position 当前玩家位置
     * @return 下一个行动玩家位置
     */
    Position nextPositionToAct(Position position) const;
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
    Action takeAction();
    /**
     * @brief 移动至下一个要行动的玩家,
     * 若下注轮已结束，则设置回合结束标志
     */
    void nextActor();

    void distributePot(const std::vector<Stack>& amounts);

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

#endif //OPENHOLDEM_GAME_STATE_H