#include "GameController.h"

#include "BettingRound.h"
#include "GameModel.h"
#include "PlayerSet.h"
#include "PotManager.h"
#include "ShowdownHandler.h"

namespace holdem {
GameController::GameController(std::unique_ptr<GameModel> model)
    : model(std::move(model)) {}

GameController::~GameController() = default;

void GameController::run() {

}
}
