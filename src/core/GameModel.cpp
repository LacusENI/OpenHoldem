#include "GameModel.h"
#include "BettingRound.h"
#include "Event.h"
#include "EventBus.h"
#include "Deck.h"
#include "eval_hand.h"
#include "Table.h"
#include "Pot.h"

namespace poker {
void join_community_cards(cards_7_t& cards7, const cards_5_t& cards5) {
    std::ranges::copy(cards5, cards7.begin());
}

void join_hole_cards(cards_7_t& cards7, const cards_2_t& cards2) {
    std::ranges::copy(cards2, cards7.begin() + 2);
}

void GameModel::takePlayerChipsToPot(seat_idx_t player_seat, chips_t chips) {
    seating_manager->getPlayerAtSeat(player_seat).bet(chips);
    pot_manager->addChips(chips);
}

void GameModel::takePotChipsToPlayer(seat_idx_t player_seat, chips_t chips) {
    pot_manager->takeChips(chips);
    seating_manager->getPlayerAtSeat(player_seat).win(chips);
}

void GameModel::applyBettingResult(seat_idx_t player_seat, algo::BettingResult result) {
    if (result.is_target_bet_valid) {
        takePlayerChipsToPot(player_seat, result.bet_delta);
        betting_round->applyBettingResult(result);
    }
}

GameModel::GameModel(
    std::shared_ptr<EventBus> event_bus,
    std::unique_ptr<IDeck> deck,
    std::shared_ptr<Pot> pot_manager,
    std::shared_ptr<Table> seating_manager,
    std::unique_ptr<BettingRound> betting_round
    ) : event_bus(std::move(event_bus)), deck(std::move(deck)), seating_manager(std::move(seating_manager)),
        pot_manager(std::move(pot_manager)),
        betting_round(std::move(betting_round)) {}

GameModel::~GameModel() = default;

Street GameModel::getStreet() const {
    return street;
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
    street = Street::IDLE;
    deck->shuffle();
    pot_manager->clearPot();
    betting_round->onGameStarted(seating_manager->getPlayerCount());
    event_bus->publish(Event::GameStarted{
        seating_manager->getBtnSeat()
    });
}

void GameModel::dealHoleCards() {
    for (Player& player : seating_manager->getPlayerList()) {
        cards_2_t hole_cards = { deck->deal(), deck->deal() };
        player.hole_cards = hole_cards;
        event_bus->publish(Event::HoleCardsDealt{
            .player_seat = player.position,
            .hole_cards = hole_cards
        });
    }
}

void GameModel::dealCommunityCards() {
    switch (street) {
    case Street::PREFLOP:
        break;
    case Street::FLOP:
        community_cards[0] = deck->deal();
        community_cards[1] = deck->deal();
        community_cards[2] = deck->deal();
        break;
    case Street::TURN:
        community_cards[3] = deck->deal();
        break;
    case Street::RIVER:
        community_cards[4] = deck->deal();
        break;
    default: break;
    }
    event_bus->publish(Event::CommunityCardsDealt{
        community_cards
    });
}

void GameModel::postForcedBlind() {
    // Small Blind
    seat_idx_t sb_seat = seating_manager->getSbSeat();
    takePlayerChipsToPot(sb_seat, big_blind / 2);
    event_bus->publish(Event::BlindPosted{
        .seat = seating_manager->getSbSeat(),
        .amount = big_blind / 2,
        .is_big_blind = false
    });
    // Big Blind
    seat_idx_t bb_seat = seating_manager->getBbSeat();
    takePlayerChipsToPot(bb_seat, big_blind);
    betting_round->processForcedBlind(sb_seat, bb_seat, big_blind);
    event_bus->publish(Event::BlindPosted{
        .seat = seating_manager->getBbSeat(),
        .amount = big_blind,
        .is_big_blind = true
    });
}

void GameModel::enterPreflopStreet() {
    street = Street::PREFLOP;
}

void GameModel::distributePot(const std::vector<chips_t>& amounts, const std::vector<seat_idx_t>& winners) {
    for (int i = 0; i < amounts.size(); i++) {
        seat_idx_t winner_seat = winners[i];
        takePotChipsToPlayer(winner_seat, amounts[i]);
    }
    pot_manager->clearPot();
}

void GameModel::advanceToNextStreet() {
    street = next_street(street);
}

void GameModel::startBettingRound() {
    seat_idx_t first_actor_seat = (street == Street::PREFLOP) ?
        seating_manager->getPreflopFirstActorSeat() :
        seating_manager->getPostflopFirstActorSeat();
    betting_round->onRoundStarted(first_actor_seat);
    event_bus->publish(Event::BettingRoundStarted{
        first_actor_seat
    });
}

void GameModel::awaitPlayerInput() {
    seat_idx_t actor_seat = betting_round->getActorSeat();
    const Player& actor = seating_manager->getPlayerAtSeat(actor_seat);
    algo::BettingContext context = betting_round->getBettingContext(actor.chips);
    event_bus->publish(Event::AwaitPlayerInput{
        actor_seat,
        context.player_chips,
        context.player_bet,
        context.round_bet,
        context.min_raise_delta
    });
}

void GameModel::advanceToNextActor() {
    seat_idx_t next_actor_seat = seating_manager->nextPositionToAct(betting_round->getActorSeat());
    betting_round->advanceToNextActor(next_actor_seat);
}

void GameModel::processPlayerInput(const Event::PlayerInputSubmitted& event) {
    seat_idx_t actor_seat = betting_round->getActorSeat();
    const Player& actor = seating_manager->getPlayerAtSeat(actor_seat);
    algo::BettingContext context = betting_round->getBettingContext(actor.chips);
    algo::BettingInput input = { event.target_bet, event.is_fold };
    algo::BettingResult result = algo::process_betting(context, input);
    takePlayerChipsToPot(getActorSeat(), result.bet_delta);
    betting_round->applyBettingResult(result);
}

void GameModel::processShowdown() {
    Event::ShowdownCompleted event;
    cards_7_t seven_cards;
    join_community_cards(seven_cards, community_cards);
    for (const Player& player : seating_manager->getPlayerList()) {
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
