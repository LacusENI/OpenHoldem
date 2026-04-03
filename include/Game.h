#ifndef OPENHOLDEM_GAME_H
#define OPENHOLDEM_GAME_H
#include <memory>
#include "Deck.h"
#include "HandEvaluator.h"

namespace holdem {

enum class GameState {
    INIT, PREFLOP, FLOP, TURN, RIVER, SHOWDOWN
};

typedef unsigned PlayerId;

/**
 * @brief 玩家实体类
 */
struct Player {
    PlayerId id{};
    Cards2 hole_cards;
    HandValue hand_value;
};

/**
 * @brief 负责控制一局游戏的流程
 */
class Game {
private:
    std::unique_ptr<IDeck> deck;
    std::vector<Player> players;
    Cards5 community_cards;
    GameState game_state = GameState::INIT;
    std::vector<Player *> winners;
public:
    explicit Game(std::unique_ptr<IDeck> deck) : deck(std::move(deck)) {}

    const Cards5& getCommunityCards() const { return community_cards; }
    GameState getGameState() const { return game_state; }
    const std::vector<Player>& getPlayers() const { return players; }
    const std::vector<Player *>& getWinners() const { return winners; }

    /**
     * 添加新玩家到玩家列表，并为此玩家分配ID
     * @param new_player 新玩家
     */
    void addPlayer(Player new_player) {
        new_player.id = players.size() + 1;
        players.push_back(new_player);
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
     * @brief 进入下一个下注轮，翻开此轮的公共牌
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