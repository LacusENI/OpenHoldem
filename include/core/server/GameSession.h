#pragma once

#include "BettingRound.h"
#include "CardsManager.h"
#include "PotManager.h"
#include "Deck.hpp"
#include "EventBus.hpp"
#include "GameController.h"
#include "GameModel.h"
#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
/**
 * @brief 会话组合根
 * @details 拥有全部游戏组件，负责构造顺序与生命周期，并以引用注入依赖。
 *          IDeck 由外部注入（调用方拥有），以便测试注入 MockDeck。
 */
struct GameSession {
    EventBus event_bus;
    PlayerSet player_set;
    SeatManager seat_manager{player_set};
    BettingRound betting_round{player_set, seat_manager};
    CardsManager cards_manager;
    PotManager pot_manager{player_set, seat_manager};
    GameModel model;
    GameController controller;

    explicit GameSession(IDeck& deck)
        : cards_manager(deck, player_set),
          model(GameDependencies{event_bus, betting_round, cards_manager, pot_manager, player_set,
                                 seat_manager}),
          controller(event_bus, model) {}

    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
};
} // namespace poker
