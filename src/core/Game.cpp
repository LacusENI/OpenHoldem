#include "Game.h"
#include "ShowdownHandler.h"

namespace holdem {
void Game::run() {
    // 初始化
    model.setup();

    view->onGameStarted();

    // 进入 Preflop 阶段
    nextStreet();

    // 执行盲注
    Action sb = model.smallBlind();
    Action bb = model.bigBlind();
    view->onPlayerActed(sb);
    view->onPlayerActed(bb);

    // 依次经过 Flop/Turn/River 阶段
    while (model.game_state != GameState::AWARD) {
        view->onRoundStarted(model);
        while (!model.is_round_ended) {
            Action action = view->onPlayerTurn(model.current_position);
            action = model.takeAction(action);
            view->onPlayerActed(action);
            model.nextActor();
        }
        nextStreet();
    }
    // 摊牌阶段
    handleAward();

    // 打印最终结果
    view->onGameOver(model);
}

void Game::handleAward() {
    auto hand_values = ShowdownHandler::evalHandValues(
        model.players, model.community_cards);
    model.winners = ShowdownHandler::determineWinners(
        hand_values);
    auto amounts = ShowdownHandler::calculateDistribution(
        model.pot, model.winners.size());
    view->onShowdownCompleted(model, hand_values);
    view->onWinnerDeclared(model.winners, amounts);
    model.distributePot(amounts);
}
}
