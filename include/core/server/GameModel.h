#pragma once

#include <vector>

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class EventBus;
class BettingRound;
class CardsManager;
class PotManager;
class PlayerSet;
class SeatManager;

/** GameModel 的依赖集合（引用注入；由组合根构造） */
struct GameDependencies {
    EventBus& event_bus;
    BettingRound& betting_round;
    CardsManager& cards_manager;
    PotManager& pot_manager;
    PlayerSet& player_set;
    SeatManager& seat_manager;
};

/**
 * @brief 游戏领域模型
 */
class GameModel {
private:
    EventBus& event_bus;
    BettingRound& betting_round;
    CardsManager& cards_manager;
    PotManager& pot_manager;
    PlayerSet& player_set;
    SeatManager& seat_manager;
    Stage current_stage = Stage::IDLE;

    /* 广播当前会话信息（按钮位、大盲、人数） */
    void publishGameStarted();

public:
    explicit GameModel(GameDependencies deps)
        : event_bus(deps.event_bus),
          betting_round(deps.betting_round),
          cards_manager(deps.cards_manager),
          pot_manager(deps.pot_manager),
          player_set(deps.player_set),
          seat_manager(deps.seat_manager) {}
    ~GameModel() = default;

    Stage getCurrentStage() const { return current_stage; }
    void setCurrentStage(Stage stage) { current_stage = stage; }

    EventBus& getEventBus() const { return event_bus; }
    BettingRound& getBettingRound() const { return betting_round; }
    CardsManager& getCardsManager() const { return cards_manager; }
    PotManager& getPotManager() const { return pot_manager; }
    PlayerSet& getPlayerSet() const { return player_set; }
    SeatManager& getSeatManager() const { return seat_manager; }

    /* 改变游戏阶段 */
    void changeStage(Stage new_stage);
    /* 移至下一个阶段 */
    void advanceStage() { changeStage(next_stage(current_stage)); }

    /* 加入新玩家 */
    void joinNewPlayer();

    /* 开始一局游戏（会话开始，初始化第一手牌状态）；玩家不足 2 人返回 false */
    bool startGame();
    /* 开始下一手牌：重置牌局状态并轮转按钮；在局玩家不足 2 人返回 false */
    bool startNextHand();
    /* 重置为一手牌开始前的状态（不轮转按钮、不发布事件） */
    void resetForNewHand();
    /* 结束本局游戏 */
    void finishGame();
    /* 洗牌 */
    void shuffleDeck();
    /* 给玩家发放底牌 */
    void dealHoleCards();
    /* 发放公共牌 */
    void dealCommunityCards();
    /* 执行小盲注 */
    void postSmallBlind();
    /* 执行大盲注 */
    void postBigBlind();

    /* 开始下注轮 */
    void startBettingRound();
    /* 初始化本轮行动顺序（应在盲注投入之后调用） */
    void startActionRound();
    /* 关闭下注轮 */
    void closeBettingRound();
    /* 退还被唯一最高下注者未被跟注的多余筹码 */
    void returnUncalledBet();
    /* 等待玩家行动 */
    void awaitPlayerAction();
    /* 处理玩家行动；返回该行动是否被接受（无效输入返回 false） */
    bool handlePlayerAction(const Event::PlayerAction& event);
    /* 移至下一个行动的玩家 */
    void moveActingPlayer();

    /* 揭露所有玩家的底牌（全下摊牌：一次全部摊开） */
    void revealAllHoleCards();
    /* 按摊牌顺序逐个揭露底牌（非全下摊牌，每次只摊一人） */
    void revealHoleCardsInOrder();
    /* 摊牌亮牌：存在全下玩家时一次全亮，否则按摊牌顺序逐个亮出 */
    void revealHoleCards();
    /* 摊牌顺序：自最后激进者（无则按钮左侧）起顺时针的在局玩家 */
    std::vector<seat_t> showdownOrder() const;
    /* 计算所有的底池 */
    void splitPot();
    /* 无人竞争时结算：整池判给唯一存活玩家 */
    void settleUncontested();
    /* 瓜分所有的底池 */
    void distributePots();

    /* 下注轮是否已结束 */
    bool isBettingRoundEnded() const;
    /* 是否只有一名玩家留有手牌 */
    bool isOnlyOnePlayerAlive() const;
    /* 所有下注轮是否已经结束 */
    bool isStreetsFinished() const;
    /* 没有任何玩家留在牌局 */
    bool isNoOnePlayerAlive() const;
    /* 是否存在仍在牌局的 all-in 玩家 */
    bool hasAllInPlayer() const;
    /* 是否满足开始一手牌的条件（至少两名在局玩家） */
    bool canStartHand() const;
};
} // namespace poker
