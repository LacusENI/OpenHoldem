#include "Game.h"

#include "BettingRound.h"
#include "IGameView.h"
#include "GameModel.h"
#include "PlayerSet.h"
#include "PotManager.h"
#include "ShowdownHandler.h"

namespace holdem {
Game::Game(std::unique_ptr<GameModel> model, std::unique_ptr<IGameView> view)
    : model(std::move(model)), view(std::move(view)) {}

Game::~Game() = default;

void Game::run() {
    // 初始化
    model->startGame();

    view->onGameStarted({});

    // 进入 Preflop 阶段
    model->nextStreet();

    // 执行盲注
    Action sb = model->smallBlind();
    Action bb = model->bigBlind();
    view->onPlayerActed({sb});
    view->onPlayerActed({bb});

    // 依次经过 Flop/Turn/River 阶段
    while (model->game_state != GameState::AWARD) {
        view->onRoundStarted({*model});
        while (!model->isRoundEnded()) {
            OnPlayerTurnData on_player_turn_data = {};
            ActionPrompt prompt = model->getActionPrompt();
            on_player_turn_data.position = prompt.position;
            on_player_turn_data.chips_to_call = prompt.chips_to_call;
            Action action = view->onPlayerTurn(on_player_turn_data).action;
            action = model->takeAction(action);
            view->onPlayerActed({action});
        }
        model->nextStreet();
    }
    // 摊牌阶段

    // 评估手牌
    auto hand_values = ShowdownHandler::evalHandValues(
        model->players->getPlayers(), model->community_cards);
    // 计算赢家
    auto winners = ShowdownHandler::determineWinners(
        hand_values);
    // 计算奖金
    auto amounts = ShowdownHandler::calculateDistribution(
        model->pot_manager->get(), winners.size());
    view->onShowdownCompleted({*model, hand_values});
    view->onWinnerDeclared({winners, amounts});
    model->distributePot(amounts, winners);

    // 打印最终结果
    view->onGameOver({*model});
}
}
