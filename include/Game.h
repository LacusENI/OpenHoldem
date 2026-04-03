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

    void addPlayer(Player new_player) {
        new_player.id = players.size();
        players.push_back(new_player);
    }

    /* 开始发放底牌 */
    void dealHoleCards();
    /* 进入下一个下注轮 */
    void nextStreet();
    /* 渲染控制台界面 */
    void render();
    /* 进入摊牌阶段，输出各玩家的牌型和胜者 */
    void showdown();
    /* 运行一局游戏 */
    void run();
};
}

#endif //OPENHOLDEM_GAME_H