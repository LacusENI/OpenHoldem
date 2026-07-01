#include "GameModel.h"

#include "BettingRound.h"
#include "Deck.h"
#include "Event.h"
#include "EventBus.h"
#include "PotManager.h"
#include "SeatManager.h"
#include "eval_hand.h"

namespace poker {
void join_community_cards(cards_7_t& cards7, const cards_5_t& cards5) {
    std::ranges::copy(cards5, cards7.begin());
}

void join_hole_cards(cards_7_t& cards7, const cards_2_t& cards2) {
    std::ranges::copy(cards2, cards7.begin() + 2);
}

void GameModel::takePlayerChipsToPot(seat_idx_t player_seat, chips_t chips) {
    seat_manager->getPlayerAtSeat(player_seat).bet(chips);
    pot_manager->addChips(chips);
}

void GameModel::takePotChipsToPlayer(seat_idx_t player_seat, chips_t chips) {
    pot_manager->takeChips(chips);
    seat_manager->getPlayerAtSeat(player_seat).win(chips);
}

void GameModel::applyBettingResult(seat_idx_t player_seat, algo::BettingResult result) {
    if (result.is_target_bet_valid) {
        takePlayerChipsToPot(player_seat, result.bet_delta);
        betting_round->applyBettingResult(result);
    }
}

GameModel::GameModel(std::shared_ptr<EventBus> event_bus, std::unique_ptr<IDeck> deck,
                     std::shared_ptr<PotManager> pot_manager,
                     std::shared_ptr<SeatManager> seat_manager,
                     std::unique_ptr<BettingRound> betting_round)
    : event_bus(std::move(event_bus)),
      deck(std::move(deck)),
      pot_manager(std::move(pot_manager)),
      seat_manager(std::move(seat_manager)),
      betting_round(std::move(betting_round)) {}

GameModel::~GameModel() = default;

Stage GameModel::getCurrentStage() const {
    return current_stage;
}

void GameModel::setCurrentStage(Stage next_street) {
    current_stage = next_street;
}

chips_t GameModel::getBigBlind() const {
    return big_blind;
}

cards_5_t GameModel::getCommunityCards() const {
    return community_cards;
}

seat_idx_t GameModel::getActorSeat() const {
    return betting_round->getActorSeat();
}

bool GameModel::isRoundFinished() const {
    return betting_round->isRoundFinished();
}

void GameModel::startGame() {
    current_stage = Stage::IDLE;
    deck->shuffle();
    pot_manager->clearPot();
    betting_round->onGameStarted(seat_manager->getPlayerCount());
}

void GameModel::dealHoleCards() {
    for (Player& player : seat_manager->getPlayerList()) {
        cards_2_t hole_cards = {deck->deal(), deck->deal()};
        player.hole_cards = hole_cards;
        event_bus->publish(
            Event::HoleCardsDealt{.player_seat = player.position, .hole_cards = hole_cards});
    }
}

void GameModel::dealCommunityCards() {
    switch (current_stage) {
    case Stage::PREFLOP:
        break;
    case Stage::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case Stage::TURN:
        community_cards[3] = deck->deal();
        break;
    case Stage::RIVER:
        community_cards[4] = deck->deal();
        break;
    default:
        break;
    }
    event_bus->publish(Event::CommunityCardsDealt{community_cards});
}

void GameModel::postForcedBlind() {
    // Small Blind
    seat_idx_t sb_seat = seat_manager->getSbSeat();
    takePlayerChipsToPot(sb_seat, big_blind / 2);
    const Player& sb_player = seat_manager->getPlayerAtSeat(sb_seat);
    event_bus->publish(
        Event::BlindPosted{.seat = sb_seat,
                           .amount = big_blind / 2,
                           .is_big_blind = false,
                           .pot = pot_manager->getPot(),
                           .player_chips = sb_player.chips,
                           .player_bet = betting_round->getPlayerBetAtSeat(sb_seat)});
    // Big Blind
    seat_idx_t bb_seat = seat_manager->getBbSeat();
    const Player& bb_player = seat_manager->getPlayerAtSeat(bb_seat);
    takePlayerChipsToPot(bb_seat, big_blind);
    betting_round->processForcedBlind(sb_seat, bb_seat, big_blind);
    event_bus->publish(
        Event::BlindPosted{.seat = bb_seat,
                           .amount = big_blind,
                           .is_big_blind = true,
                           .pot = pot_manager->getPot(),
                           .player_chips = bb_player.chips,
                           .player_bet = betting_round->getPlayerBetAtSeat(bb_seat)});
}

void GameModel::enterPreflopStreet() {
    current_stage = Stage::PREFLOP;
}

void GameModel::distributePot(const std::vector<chips_t>& amounts,
                              const std::vector<seat_idx_t>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        seat_idx_t winner_seat = winners[i];
        takePotChipsToPlayer(winner_seat, amounts[i]);
    }
    pot_manager->clearPot();
}

void GameModel::advanceToNextStreet() {
    current_stage = next_stage(current_stage);
}

void GameModel::setupRoundStart() {
    seat_idx_t first_actor_seat = (current_stage == Stage::PREFLOP)
                                      ? seat_manager->getPreflopFirstActorSeat()
                                      : seat_manager->getPostflopFirstActorSeat();
    betting_round->onRoundStarted(first_actor_seat);
}

void GameModel::awaitPlayerInput() {
    seat_idx_t actor_seat = betting_round->getActorSeat();
    const Player& actor = seat_manager->getPlayerAtSeat(actor_seat);
    algo::BettingContext context = betting_round->getBettingContext(actor.chips);
    event_bus->publish(Event::AwaitPlayerInput{actor_seat, context.player_chips, context.player_bet,
                                               context.round_bet, context.min_raise_delta});
}

void GameModel::advanceToNextActor() {
    seat_idx_t next_actor_seat = seat_manager->findNextActorSeat(betting_round->getActorSeat());
    betting_round->advanceToNextActor(next_actor_seat);
}

void GameModel::processPlayerInput(const Event::PlayerInputSubmitted& event) {
    seat_idx_t actor_seat = betting_round->getActorSeat();
    const Player& actor = seat_manager->getPlayerAtSeat(actor_seat);
    algo::BettingContext context = betting_round->getBettingContext(actor.chips);
    algo::BettingInput input = {event.target_bet, event.is_fold};
    algo::BettingResult result = algo::process_betting(context, input);
    takePlayerChipsToPot(getActorSeat(), result.bet_delta);
    betting_round->applyBettingResult(result);
    event_bus->publish(
        Event::PlayerActed{.actor_seat = actor_seat,
                           .action = result.action,
                           .pot = pot_manager->getPot(),
                           .player_chips = seat_manager->getPlayerAtSeat(actor_seat).chips,
                           .player_bet = result.player_bet});
}

void GameModel::revealHoleCards() {
    Event::HoleCardsRevealed event;
    cards_7_t seven_cards;
    join_community_cards(seven_cards, community_cards);
    for (const Player& player : seat_manager->getPlayerList()) {
        if (player.isActive()) {
            join_hole_cards(seven_cards, player.hole_cards);
            HandValue hand_value = algo::eval_hand(seven_cards);
            event.hand_values.push_back(hand_value);
            event.active_players.push_back(player.position);
            event.hole_cards.push_back(player.hole_cards);
        }
    }
    event_bus->publish(event);
}
} // namespace poker
