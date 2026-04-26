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
    model->setup();

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
            Action action = view->onPlayerTurn({model->betting_round->getCurrentPosition()}).action;
            action = model->takeAction(action);
            view->onPlayerActed({action});
            model->nextActor();
        }
        model->nextStreet();
    }
    // 摊牌阶段
    handleAward();

    // 打印最终结果
    view->onGameOver({*model});
}

void Game::handleAward() {
    auto hand_values = ShowdownHandler::evalHandValues(
        model->players->getPlayers(), model->community_cards);
    auto winners = ShowdownHandler::determineWinners(
        hand_values);
    auto amounts = ShowdownHandler::calculateDistribution(
        model->pot_manager->get(), winners.size());
    view->onShowdownCompleted({*model, hand_values});
    view->onWinnerDeclared({winners, amounts});
    model->distributePot(amounts, winners);
}
}
