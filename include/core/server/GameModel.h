#pragma once

#include <optional>
#include <vector>

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class BettingRound;
class CardsManager;
class PotManager;
class PlayerSet;
class SeatManager;

/** GameModel 的依赖集合（引用注入；由组合根构造） */
struct GameDependencies {
    BettingRound& betting_round;
    CardsManager& cards_manager;
    PotManager& pot_manager;
    PlayerSet& player_set;
    SeatManager& seat_manager;
};

/**
 * @brief 游戏领域模型
 * @details 不依赖 EventBus：各方法返回"应当发布"的事件，由 ServerController 负责发布。
 *          单事件返回具体事件类型；多事件返回 std::vector<具体事件>。
 */
class GameModel {
private:
    BettingRound& betting_round;
    CardsManager& cards_manager;
    PotManager& pot_manager;
    PlayerSet& player_set;
    SeatManager& seat_manager;
    Stage current_stage = Stage::IDLE;

public:
    explicit GameModel(GameDependencies deps)
        : betting_round(deps.betting_round),
          cards_manager(deps.cards_manager),
          pot_manager(deps.pot_manager),
          player_set(deps.player_set),
          seat_manager(deps.seat_manager) {}
    ~GameModel() = default;

    Stage getCurrentStage() const { return current_stage; }
    void setCurrentStage(Stage stage) { current_stage = stage; }

    BettingRound& getBettingRound() const { return betting_round; }
    CardsManager& getCardsManager() const { return cards_manager; }
    PotManager& getPotManager() const { return pot_manager; }
    PlayerSet& getPlayerSet() const { return player_set; }
    SeatManager& getSeatManager() const { return seat_manager; }

    /* 改变游戏阶段，返回阶段变化事件 */
    Event::StageChanged changeStage(Stage new_stage);
    /* 移至下一个阶段 */
    Event::StageChanged advanceStage() { return changeStage(next_stage(current_stage)); }

    /* 加入新玩家 */
    Event::PlayerJoined joinNewPlayer();

    /* 开始一局游戏（会话开始，初始化第一手牌状态）；玩家不足 2 人返回 nullopt */
    std::optional<Event::GameStarted> startGame();
    /* 开始下一手牌：重置牌局状态并轮转按钮；在局玩家不足 2 人返回 nullopt */
    std::optional<Event::GameStarted> startNextHand();
    /* 重置为一手牌开始前的状态（不轮转按钮） */
    void resetForNewHand();
    /* 结束本局游戏 */
    Event::GameEnded finishGame();
    /* 洗牌 */
    void shuffleDeck();
    /* 给玩家发放底牌，返回每位玩家的发牌事件 */
    std::vector<Event::HoleCardsDealt> dealHoleCards();
    /* 发放公共牌 */
    Event::CommunityCardsDealt dealCommunityCards();
    /* 执行小盲注 */
    Event::BlindPosted postSmallBlind();
    /* 执行大盲注 */
    Event::BlindPosted postBigBlind();

    /* 开始下注轮 */
    void startBettingRound();
    /* 初始化本轮行动顺序（应在盲注投入之后调用） */
    void startActionRound();
    /* 关闭下注轮 */
    void closeBettingRound();
    /* 退还被唯一最高下注者未被跟注的多余筹码；无则返回 nullopt */
    std::optional<Event::UncalledBetReturned> returnUncalledBet();
    /* 等待玩家行动；无有效行动者返回 nullopt */
    std::optional<Event::TurnStarted> awaitPlayerAction();
    /* 处理玩家行动；无效输入返回 nullopt */
    std::optional<Event::PlayerActed> handlePlayerAction(const Command::SubmitAction& event);
    /* 移至下一个行动的玩家 */
    void moveActingPlayer();

    /* 揭露所有玩家的底牌（全下摊牌：一条事件含全部） */
    std::vector<Event::HoleCardsRevealed> revealAllHoleCards();
    /* 按摊牌顺序逐个揭露底牌（每次一条事件） */
    std::vector<Event::HoleCardsRevealed> revealHoleCardsInOrder();
    /* 摊牌亮牌：存在全下玩家时一次全亮，否则按摊牌顺序逐个亮出 */
    std::vector<Event::HoleCardsRevealed> revealHoleCards();
    /* 摊牌顺序：自最后激进者（无则按钮左侧）起顺时针的在局玩家 */
    std::vector<seat_t> showdownOrder() const;
    /* 计算所有的底池 */
    void splitPot();
    /* 无人竞争时结算：整池判给唯一存活玩家 */
    Event::PotAwarded settleUncontested();
    /* 瓜分所有的底池 */
    std::vector<Event::PotAwarded> distributePots();

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
