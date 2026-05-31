#include "GameController.h"

#include "BettingRound.h"
#include "GameModel.h"

namespace poker {
GameController::GameController(std::unique_ptr<GameModel> model)
    : model(std::move(model)) {}

GameController::~GameController() = default;

void GameController::run() {

}
}
