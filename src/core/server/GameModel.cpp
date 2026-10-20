#include "GameModel.h"

#include <optional>
#include <utility>
#include <vector>

#include "BettingRound.h"
#include "Card.hpp"
#include "CardsManager.h"
#include "Event.hpp"
#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "PotManager.h"
#include "SeatManager.h"

namespace poker {
Event::PlayerJoined GameModel::joinNewPlayer() {
    player_set.addNewPlayer();
    return Event::PlayerJoined{.seat = player_set.playerCount() - 1,
                               .player_count = player_set.playerCount()};
}
std::optional<Event::GameStarted> GameModel::startGame() {
    resetForNewHand();
    if (!canStartHand()) return std::nullopt;
    return Event::GameStarted{.btn_seat = seat_manager.getBtnSeat(),
                              .big_blind = betting_round.getBigBlind(),
                              .player_count = player_set.playerCount()};
}
std::optional<Event::GameStarted> GameModel::startNextHand() {
    resetForNewHand();
    if (!canStartHand()) return std::nullopt;
    seat_manager.advanceBtnSeat();
    return Event::GameStarted{.btn_seat = seat_manager.getBtnSeat(),
                              .big_blind = betting_round.getBigBlind(),
                              .player_count = player_set.playerCount()};
}
void GameModel::resetForNewHand() {
    for (Player& player : player_set) {
        player.resetForNewHand();
    }
    cards_manager.clearCommunityCards();
    pot_manager.clearPots();
    betting_round.resetBets();
}
Event::GameEnded GameModel::finishGame() {
    return Event::GameEnded{};
}
Event::StageChanged GameModel::changeStage(Stage new_stage) {
    current_stage = new_stage;
    return Event::StageChanged{new_stage};
}
void GameModel::shuffleDeck() {
    cards_manager.shuffleDeck();
}
std::vector<Event::HoleCardsDealt> GameModel::dealHoleCards() {
    cards_manager.dealHoleCards();
    std::vector<Event::HoleCardsDealt> events;
    for (seat_t seat_idx : player_set.getActivePlayers()) {
        events.push_back(Event::HoleCardsDealt{
            .player_seat = seat_idx, .hole_cards = cards_manager.getHoleCardsAt(seat_idx)});
    }
    return events;
}
Event::CommunityCardsDealt GameModel::dealCommunityCards() {
    cards_manager.dealCommunityCards(current_stage);
    return Event::CommunityCardsDealt{.community_cards = cards_manager.getCommunityCards()};
}
Event::BlindPosted GameModel::postSmallBlind() {
    seat_t sb_seat = seat_manager.getSbSeat();
    chips_t posted = betting_round.applySmallBlind(sb_seat);
    player_set.playerAt(sb_seat).payChips(posted); // payChips 会在扣至 0 时置 all-in
    return Event::BlindPosted{.player = sb_seat, .amount = posted, .is_big_blind = false};
}
Event::BlindPosted GameModel::postBigBlind() {
    seat_t bb_seat = seat_manager.getBbSeat();
    chips_t posted = betting_round.applyBigBlind(bb_seat);
    player_set.playerAt(bb_seat).payChips(posted); // payChips 会在扣至 0 时置 all-in
    return Event::BlindPosted{.player = bb_seat, .amount = posted, .is_big_blind = true};
}
void GameModel::startBettingRound() {
    betting_round.resetBets();
}
void GameModel::startActionRound() {
    betting_round.setupToStart(current_stage);
}
void GameModel::closeBettingRound() {
    betting_round.resetBets();
}
std::optional<Event::UncalledBetReturned> GameModel::returnUncalledBet() {
    auto uncalled = betting_round.findUncalledBet();
    if (!uncalled) return std::nullopt;
    player_set.playerAt(uncalled->seat).refund(uncalled->amount);
    return Event::UncalledBetReturned{.player = uncalled->seat, .amount = uncalled->amount};
}
std::optional<Event::TurnStarted> GameModel::awaitPlayerAction() {
    if (!betting_round.hasActingPlayer()) return std::nullopt; // 无有效行动者
    seat_t acting_player = betting_round.getActingPlayer();
    return Event::TurnStarted{.player = acting_player,
                              .player_chips = pot_manager.getPlayerChipsAt(acting_player),
                              .player_bet = betting_round.getPlayerBetAt(acting_player),
                              .round_bet = betting_round.getRoundBet(),
                              .min_raise = betting_round.getMinRaise(),
                              .can_raise = betting_round.canPlayerRaise(acting_player)};
}
std::optional<Event::PlayerActed> GameModel::handlePlayerAction(
    const Command::SubmitAction& event) {
    return betting_round.applyAction(event);
}
void GameModel::moveActingPlayer() {
    betting_round.moveActingPlayer();
}
std::vector<Event::HoleCardsRevealed> GameModel::revealAllHoleCards() {
    std::vector<std::pair<seat_t, cards_2_t>> players_with_hole_cards;
    for (seat_t player : player_set.getAlivePlayers()) {
        cards_2_t hole_cards = cards_manager.getHoleCardsAt(player);
        players_with_hole_cards.push_back({player, hole_cards});
    }
    return {Event::HoleCardsRevealed{players_with_hole_cards}};
}
std::vector<Event::HoleCardsRevealed> GameModel::revealHoleCardsInOrder() {
    std::vector<Event::HoleCardsRevealed> events;
    for (seat_t seat : showdownOrder()) {
        events.push_back(Event::HoleCardsRevealed{{{seat, cards_manager.getHoleCardsAt(seat)}}});
    }
    return events;
}
std::vector<Event::HoleCardsRevealed> GameModel::revealHoleCards() {
    if (hasAllInPlayer()) return revealAllHoleCards(); // 全下摊牌：一次全部摊开
    return revealHoleCardsInOrder();                   // 非全下摊牌：按顺序逐个亮出
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
    pot_manager.splitThePot();
}
Event::PotAwarded GameModel::settleUncontested() {
    // 唯一存活玩家直接赢得整池（不划分边池）
    auto alive = player_set.getAlivePlayers();
    seat_t winner = *alive.begin();
    // 整池 = 所有在局玩家（含弃牌者的死钱）的本局投入，须在 awardPotTo 清空底池前汇总
    chips_t total = 0;
    for (seat_t seat : player_set.getPlayingPlayers()) {
        total += player_set.playerAt(seat).getCommitted();
    }
    pot_manager.awardPotTo(winner);
    pot_manager.clearCommitted(); // 结算后清零本局投入
    return Event::PotAwarded{.pot_index = 0, .chips = total, .award_to = {winner}};
}
std::vector<Event::PotAwarded> GameModel::distributePots() {
    std::vector<Event::PotAwarded> events;
    int pot_count = pot_manager.getPots().size();
    for (int pot_index = pot_count - 1; pot_index >= 0; pot_index--) {
        const Pot& pot = pot_manager.getPots()[pot_index];

        std::vector<seat_t> contenders;
        for (seat_t contributor : pot.contributors) {
            if (player_set.playerAt(contributor).isAlive()) {
                contenders.push_back(contributor);
            }
        }
        std::vector<seat_t> winners = cards_manager.determineWinners(contenders);
        pot_manager.distributePot(pot.chips, winners);
        events.push_back(
            Event::PotAwarded{.pot_index = pot_index, .chips = pot.chips, .award_to = winners});
    }
    pot_manager.clearCommitted(); // 结算后清零本局投入
    return events;
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
