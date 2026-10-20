#pragma once

#include "BettingRound.h"
#include "CardsManager.h"
#include "ClientGateway.h"
#include "Deck.hpp"
#include "EventBus.hpp"
#include "ServerController.h"
#include "GameModel.h"
#include "PlayerSet.hpp"
#include "PotManager.h"
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
    ServerController controller;

    GameSession(IDeck& deck, ClientGateway& client_gateway)
        : cards_manager(deck, player_set),
          model(GameDependencies{
              .betting_round = betting_round,
              .cards_manager = cards_manager,
              .pot_manager = pot_manager,
              .player_set = player_set,
              .seat_manager = seat_manager,
          }),
          controller(event_bus, client_gateway, model) {}

    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
};
} // namespace poker
