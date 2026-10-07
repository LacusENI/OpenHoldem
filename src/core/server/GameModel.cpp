#include "GameModel.h"

#include <optional>
#include <utility>
#include <vector>

#include "BettingRound.h"
#include "Card.hpp"
#include "CardsManager.h"
#include "ChipsManager.h"
#include "Event.hpp"
#include "EventBus.hpp"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"

namespace poker {
void GameModel::joinNewPlayer() {
    player_set.addNewPlayer();
    event_bus.publish(Event::NewPlayerJoined{.new_seat = player_set.playerCount() - 1,
                                              .player_count = player_set.playerCount()});
}
bool GameModel::startGame() {
    resetForNewHand();
    if (!canStartHand()) return false;
    publishGameStarted();
    return true;
}
bool GameModel::startNextHand() {
    resetForNewHand();
    if (!canStartHand()) return false;
    seat_manager.advanceBtnSeat();
    publishGameStarted();
    return true;
}
void GameModel::resetForNewHand() {
    for (Player& player : player_set) {
        player.resetForNewHand();
    }
    cards_manager.clearCommunityCards();
    chips_manager.clearPots();
    betting_round.resetBets();
}
void GameModel::publishGameStarted() {
    event_bus.publish(Event::GameStarted{.btn_seat = seat_manager.getBtnSeat(),
                                          .big_blind = betting_round.getBigBlind(),
                                          .player_count = player_set.playerCount()});
}
void GameModel::finishGame() {
    event_bus.publish(Event::GameEnded{});
}
void GameModel::changeStage(Stage new_stage) {
    current_stage = new_stage;
    event_bus.publish(Event::StageChanged{new_stage});
}
void GameModel::shuffleDeck() {
    cards_manager.shuffleDeck();
}
void GameModel::dealHoleCards() {
    cards_manager.dealHoleCards();
    for (seat_t seat_idx : player_set.getActivePlayers()) {
        event_bus.publish(Event::HoleCardsDealt{
            .player_seat = seat_idx, .hole_cards = cards_manager.getHoleCardsAt(seat_idx)});
    }
}
void GameModel::dealCommunityCards() {
    cards_manager.dealCommunityCards(current_stage);
    event_bus.publish(
        Event::CommunityCardsDealt{.community_cards = cards_manager.getCommunityCards()});
}
void GameModel::postSmallBlind() {
    seat_t sb_seat = seat_manager.getSbSeat();
    chips_t posted = betting_round.applySmallBlind(sb_seat);
    chips_manager.moveChipsToPot(sb_seat, posted); // payChips 会在扣至 0 时置 all-in
    event_bus.publish(
        Event::BlindPosted{.player = sb_seat, .amount = posted, .is_big_blind = false});
}
void GameModel::postBigBlind() {
    seat_t bb_seat = seat_manager.getBbSeat();
    chips_t posted = betting_round.applyBigBlind(bb_seat);
    chips_manager.moveChipsToPot(bb_seat, posted); // payChips 会在扣至 0 时置 all-in
    event_bus.publish(
        Event::BlindPosted{.player = bb_seat, .amount = posted, .is_big_blind = true});
}
void GameModel::startBettingRound() {
    betting_round.resetBets();
    event_bus.publish(Event::BettingRoundStarted{.stage = current_stage});
}
void GameModel::startActionRound() {
    betting_round.setupToStart(current_stage);
}
void GameModel::closeBettingRound() {
    betting_round.resetBets();
    event_bus.publish(Event::BettingRoundEnded{.stage = current_stage});
}
void GameModel::returnUncalledBet() {
    auto uncalled = betting_round.findUncalledBet();
    if (!uncalled) return;
    player_set.playerAt(uncalled->seat).refund(uncalled->amount);
    event_bus.publish(
        Event::UncalledBetReturned{.player = uncalled->seat, .amount = uncalled->amount});
}
void GameModel::awaitPlayerAction() {
    if (!betting_round.hasActingPlayer()) return; // 无有效行动者，不可请求行动

    seat_t acting_player = betting_round.getActingPlayer();
    chips_t player_chips = chips_manager.getPlayerChipsAt(acting_player);
    chips_t player_bet = betting_round.getPlayerBetAt(acting_player);
    chips_t round_bet = betting_round.getRoundBet();
    chips_t min_raise = betting_round.getMinRaise();
    bool can_raise = betting_round.canPlayerRaise(acting_player);
    Event::WaitingForAction event = {.player = acting_player,
                                     .player_chips = player_chips,
                                     .player_bet = player_bet,
                                     .round_bet = round_bet,
                                     .min_raise = min_raise,
                                     .can_raise = can_raise};
    event_bus.publish(event);
}
bool GameModel::handlePlayerAction(const Event::PlayerAction& event) {
    // 行动的处理（构建下注上下文、落子、更新行动顺序）由 BettingRound 负责
    std::optional<Event::PlayerActed> acted = betting_round.applyAction(event);
    if (!acted) return false; // 无效输入：由调用方重新请求该玩家行动
    event_bus.publish(*acted);
    return true;
}
void GameModel::moveActingPlayer() {
    betting_round.moveActingPlayer();
}
void GameModel::revealAllHoleCards() {
    std::vector<std::pair<seat_t, cards_2_t>> players_with_hole_cards;
    for (seat_t player : player_set.getAlivePlayers()) {
        cards_2_t hole_cards = cards_manager.getHoleCardsAt(player);
        players_with_hole_cards.push_back({player, hole_cards});
    }
    event_bus.publish(Event::HoleCardsRevealed{players_with_hole_cards});
}
void GameModel::revealHoleCardsInOrder() {
    for (seat_t seat : showdownOrder()) {
        cards_2_t hole_cards = cards_manager.getHoleCardsAt(seat);
        event_bus.publish(Event::HoleCardsRevealed{{{seat, hole_cards}}});
    }
}
void GameModel::revealHoleCards() {
    if (hasAllInPlayer()) {
        revealAllHoleCards(); // 全下摊牌：一次全部摊开
    } else {
        revealHoleCardsInOrder(); // 非全下摊牌：按顺序逐个亮出
    }
}
std::vector<seat_t> GameModel::showdownOrder() const {
    std::vector<seat_t> order;
    // 起始：河牌轮最后激进者；若本轮无人下注，则按钮左侧第一个活跃玩家
    seat_t first = betting_round.getLastAggressor();
    if (first == INVALID_SEAT) {
        first = seat_manager.nextActiveSeat(seat_manager.getBtnSeat());
    }
    if (first == INVALID_SEAT) return order;

    seat_t seat = first;
    do {
        if (player_set.playerAt(seat).isAlive()) order.push_back(seat);
        seat = seat_manager.nextSeat(seat);
    } while (seat != first);
    return order;
}
void GameModel::splitPot() {
    chips_manager.splitThePot(); 
    std::vector<Pot> pots = chips_manager.getPots();
    std::vector<chips_t> pot_chips;
    for (const Pot& pot : pots) {
        pot_chips.push_back(pot.chips);
    }
    event_bus.publish(Event::PotSplited{.pot_chips = pot_chips});
}
void GameModel::settleUncontested() {
    // 唯一存活玩家直接赢得整池（不划分边池，也不产生"单人池"
    // 被当作赢池上报的问题）
    auto alive = player_set.getAlivePlayers();
    seat_t winner = *alive.begin();
    chips_manager.awardPotTo(winner);
    chips_manager.clearCommitted(); // 结算后清零本局投入
    event_bus.publish(Event::WinnerDeclared{.pot_index = 0, .winners = {winner}});
}
void GameModel::distributePots() {
    int pot_count = chips_manager.getPots().size();
    for (int pot_index = pot_count - 1; pot_index >= 0; pot_index--) {
        const Pot& pot = chips_manager.getPots()[pot_index];

        std::vector<seat_t> contenders;
        for (seat_t contributor : pot.contributors) {
            if (player_set.playerAt(contributor).isAlive()) {
                contenders.push_back(contributor);
            }
        }
        std::vector<seat_t> winners = cards_manager.determineWinners(contenders);
        chips_manager.distributePot(pot.chips, winners);
        event_bus.publish(Event::WinnerDeclared{.pot_index = pot_index, .winners = winners});
    }
    chips_manager.clearCommitted(); // 结算后清零本局投入
}
bool GameModel::isBettingRoundEnded() const {
    return betting_round.isFlowEnded();
}
bool GameModel::isOnlyOnePlayerAlive() const {
    return player_set.getAlivePlayerCount() == 1;
}
bool GameModel::isStreetsFinished() const {
    return player_set.getActivePlayerCount() <= 1;
}
bool GameModel::isNoOnePlayerAlive() const {
    return player_set.getAlivePlayerCount() == 0;
}
bool GameModel::hasAllInPlayer() const {
    for (seat_t seat = 0; seat < player_set.playerCount(); ++seat) {
        if (player_set.playerAt(seat).isAllIn()) return true;
    }
    return false;
}
bool GameModel::canStartHand() const {
    return player_set.getAlivePlayerCount() >= 2;
}
} // namespace poker
