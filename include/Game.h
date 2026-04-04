#ifndef OPENHOLDEM_GAME_H
#define OPENHOLDEM_GAME_H
#include <memory>
#include "Deck.h"
#include "HandEvaluator.h"

namespace holdem {

enum class GameState {
    INIT, PREFLOP, FLOP, TURN, RIVER, SHOWDOWN
};

using PlayerId = int; // 玩家ID
using Position = int; // 玩家座位
using Stack = int; // 筹码数量

/**
 * @brief 玩家实体类
 */
struct Player {
    PlayerId id{};       // 玩家ID
    Position position{}; // 玩家所在位置
    Cards2 hole_cards;   // 玩家的底牌

    Stack chips = 0;       // 玩家拥有的筹码
    Stack current_bet = 0; // 玩家当前下注额

    HandValue hand_value; // 手牌分值
};

/**
 * @brief 负责控制一局游戏的流程
 */
class Game {
private:
    GameState game_state = GameState::INIT; // 游戏状态机

    Position button_position = 0; // 庄家位置
    Stack big_blind = 10;         // 大盲注额
    Stack pot = 0;                // 底池金额
    Stack current_bet = 0;        // 当前下注额
    Position rest_position = 0;   // 休止符玩家位置(若该玩家行动后下一位是休止符玩家，本轮下注结束)

    std::unique_ptr<IDeck> deck; // 牌组
    Cards5 community_cards;      // 公共牌
    std::vector<Player> players; // 玩家列表

    std::vector<Position> winners{}; // 赢家列表
public:
    explicit Game(std::unique_ptr<IDeck> deck) : deck(std::move(deck)) {}

    const Cards5& getCommunityCards() const { return community_cards; }
    GameState getGameState() const { return game_state; }
    const std::vector<Player>& getPlayers() const { return players; }
    Position getButtonPosition() const { return button_position; }
    Stack getBigBlind() const { return big_blind; }
    Stack getPot() const { return pot; }
    Stack getCurrentBet() const { return current_bet; }
    const std::vector<Position>& getWinners() const { return winners; }

    /**
     * @brief 添加新玩家到玩家列表
     * @param id 玩家的ID
     */
    void addPlayer(PlayerId id) {
        Player new_player;
        new_player.id = id;
        new_player.position = static_cast<int>(players.size());
        players.push_back(new_player);
    }
    /**
     * 获取到该座位上的玩家信息
     * @param position 玩家座位(从0开始)
     * @return 玩家
     */
    Player& getPlayerByPosition(Position position) {
        return players[position];
    }
    /**
     * 通过 ID 搜索玩家
     * @param id 玩家ID
     * @return 目标玩家
     */
    Player& getPlayerById(PlayerId id) {
        for (Player& player : players) {
            if (player.id == id) return player;
        }
        throw std::runtime_error("Player not found");
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
     * @param position 玩家位置
     * @param amount 投入筹码的数额
     */
    void commitChips(Position position, Stack amount) {
        Player& player = getPlayerByPosition(position);
        player.chips -= amount;
        pot += amount;
        player.current_bet += amount;
        if (player.current_bet > current_bet) {
            current_bet = player.current_bet;
        }
    }

    /**
     * @brief 运行一局游戏
     * @details 调用此函数后，将进行一局游戏，直到结束
     */
    void run();
    /**
     * @brief 为每位玩家发放底牌
     * @note 每位玩家一次性发放两张底牌
     */
    void dealHoleCards();
    /**
     * @brief 投入小盲注和大盲注
     */
    void postBlind();
    /**
     * @brief 进入下一个阶段，翻开此轮的公共牌
     * @note 每次发公共牌前不烧牌
     */
    void nextStreet();
    /**
     * @brief 渲染控制台界面
     */
    void render();
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
     * @brief 该玩家开始行动
     * @note demo_v2 阶段只有"投入大盲注等额"一种行动
     */
    void takeAction(Position position);
    /**
     * @brief 为赢家分配底池
     * @note 若有多名玩家获胜且底池无法均分,
     * 则座位靠前的赢家比靠后的赢家多分得一个筹码
     */
    void award();
};

namespace internal {
/**
 * 返回用于用户界面的牌型描述信息，例如:
 * - Straight Flush, 9 to King
 * - Full House, 6s full of Jacks
 * @param hand_value 牌型值
 * @return 牌型的描述信息
 */
std::string getHandMessage(HandValue hand_value);
} // namespace holdem::internal

} // namespace holdem

#endif //OPENHOLDEM_GAME_H