#include "GameModel.h"

#include <ranges>
#include <utility>
#include <vector>

#include "ActionFlow.h"
#include "BettingManager.h"
#include "Card.hpp"
#include "CardsManager.h"
#include "ChipsManager.h"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
void GameModel::joinNewPlayer() {
    player_set->addNewPlayer();
    event_bus->publish(Event::NewPlayerJoined{.new_seat = player_set->playerCount() - 1,
                                              .player_count = player_set->playerCount()});
}
void GameModel::startGame() {
    event_bus->publish(Event::GameStarted{});
}
void GameModel::finishGame() {
    event_bus->publish(Event::GameEnded{});
}
void GameModel::changeStage(Stage new_stage) {
    current_stage = new_stage;
    event_bus->publish(Event::StageChanged{new_stage});
}
void GameModel::shuffleDeck() {
    cards_manager->shuffleDeck();
}
void GameModel::dealHoleCards() {
    cards_manager->dealHoleCards();
    for (seat_t seat_idx : player_set->getActivePlayers()) {
        event_bus->publish(Event::HoleCardsDealt{
            .player_seat = seat_idx, .hole_cards = cards_manager->getHoleCardsAt(seat_idx)});
    }
}
void GameModel::dealCommunityCards() {
    cards_manager->dealCommunityCards(current_stage);
    event_bus->publish(
        Event::CommunityCardsDealt{.community_cards = cards_manager->getCommunityCards()});
}
void GameModel::postSmallBlind() {
    seat_t sb_seat = seat_manager->getSbSeat();
    chips_t small_blind = betting_manager->getSmallBlind();
    chips_manager->moveChipsToPot(sb_seat, small_blind);
    betting_manager->applySmallBlind(sb_seat);
    event_bus->publish(
        Event::BlindPosted{.player = sb_seat, .amount = small_blind, .is_big_blind = false});
}
void GameModel::postBigBlind() {
    seat_t bb_seat = seat_manager->getBbSeat();
    chips_t big_blind = betting_manager->getBigBlind();
    chips_manager->moveChipsToPot(bb_seat, big_blind);
    betting_manager->applyBigBlind(bb_seat);
    event_bus->publish(
        Event::BlindPosted{.player = bb_seat, .amount = big_blind, .is_big_blind = true});
}
void GameModel::startBettingRound() {
    betting_manager->setupBetting();
    action_flow->setupToStart(current_stage);
    event_bus->publish(Event::BettingRoundStarted{});
}
void GameModel::closeBettingRound() {
    betting_manager->setupBetting();
    event_bus->publish(Event::BettingRoundEnded{});
}
void GameModel::awaitPlayerAction() {
    seat_t acting_player = action_flow->getActingPlayer();
    chips_t player_chips = chips_manager->getPlayerChipsAt(acting_player);
    chips_t player_bet = betting_manager->getPlayerBetAt(acting_player);
    chips_t round_bet = betting_manager->getRoundBet();
    chips_t min_raise = betting_manager->getMinRaise();
    bool can_raise = action_flow->canPlayerRaise(acting_player);
    Event::WaitingForAction event = {.player = acting_player,
                                     .player_chips = player_chips,
                                     .player_bet = player_bet,
                                     .round_bet = round_bet,
                                     .min_raise = min_raise,
                                     .can_raise = can_raise};
    event_bus->publish(event);
}
void GameModel::handlePlayerAction(const Event::PlayerAction& event) {
    seat_t acting_player = action_flow->getActingPlayer();

    if (event.is_fold) {
        player_set->playerAt(acting_player).fold();
        action_flow->applyAction(Action::FOLD);
        event_bus->publish(
            Event::PlayerActed{.player = acting_player, .bet = 0, .action = Action::FOLD});
        return;
    }
    algo::BettingContext context = {.player_chips = chips_manager->getPlayerChipsAt(acting_player),
                                    .player_bet = betting_manager->getPlayerBetAt(acting_player),
                                    .round_bet = betting_manager->getRoundBet(),
                                    .min_raise_delta = betting_manager->getMinRaiseDelta(),
                                    .can_raise = action_flow->canPlayerRaise(acting_player)};
    algo::BettingInput input = {.target_bet = event.target_bet, .is_fold = event.is_fold};
    algo::BettingResult result = algo::process_betting(context, input);

    if (result.isValid()) {
        chips_manager->moveChipsToPot(acting_player, result.bet_delta);
        betting_manager->setPlayerBetAt(acting_player, result.player_bet);
        betting_manager->setMinRaiseDelta(result.min_raise_delta);
        betting_manager->setRoundBet(result.round_bet);
        action_flow->applyAction(result.action);
        event_bus->publish(Event::PlayerActed{
            .player = acting_player, .bet = result.player_bet, .action = result.action});
    }
    // TODO: 增加输入无效时的信息返回
}
void GameModel::moveActingPlayer() {
    action_flow->moveActingPlayer();
}
void GameModel::revealAllHoleCards() {
    std::vector<std::pair<seat_t, cards_2_t>> players_with_hole_cards;
    for (seat_t player : player_set->getActivePlayers()) {
        cards_2_t hole_cards = cards_manager->getHoleCardsAt(player);
        players_with_hole_cards.push_back({player, hole_cards});
    }
    event_bus->publish(Event::HoleCardsRevealed{players_with_hole_cards});
}
void GameModel::splitPot() {
    chips_manager->splitThePot(); 
    std::vector<Pot> pots = chips_manager->getPots();
    std::vector<chips_t> pot_chips;
    for (const Pot& pot : pots) {
        pot_chips.push_back(pot.chips);
    }
    event_bus->publish(Event::PotSplited{.pot_chips = pot_chips});
}
void GameModel::distributePots() {
    int pot_count = chips_manager->getPots().size();
    for (int pot_index = pot_count - 1; pot_index >= 0; pot_index--) {
        const Pot& pot = chips_manager->getPots()[pot_index];

        std::vector<seat_t> contenders;
        for (seat_t contributor : pot.contributors) {
            if (player_set->playerAt(contributor).isAlive()) {
                contenders.push_back(contributor);
            }
        }
        std::vector<seat_t> winners = cards_manager->determineWinners(contenders);
        chips_manager->distributePot(pot.chips, winners);
        event_bus->publish(Event::WinnerDeclared{.pot_index = pot_index, .winners = winners});
    }
}
std::vector<seat_t> GameModel::comparePlayerHands() {
    auto view = player_set->getAlivePlayers();
    std::vector<seat_t> players(view.begin(), view.end());
    return cards_manager->determineWinners(players);
}
bool GameModel::isBettingRoundEnded() const {
    return action_flow->isFlowEnded();
}
bool GameModel::isOnlyOnePlayerAlive() const {
    return player_set->getAlivePlayerCount() == 1;
}
bool GameModel::isStreetsFinished() const {
    return player_set->getActivePlayerCount() <= 1;
}
bool GameModel::isNoOnePlayerAlive() const {
    return player_set->getAlivePlayerCount() == 0;
}
} // namespace poker
