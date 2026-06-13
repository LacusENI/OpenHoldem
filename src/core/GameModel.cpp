#include "GameModel.h"
#include "BettingRound.h"
#include "Event.h"
#include "EventBus.h"
#include "Deck.h"
#include "SeatingManager.h"
#include "PotManager.h"

namespace poker {
void GameModel::takePlayerChipsToPot(seat_idx_t player_seat, chips_t chips) {
    seating_manager->getPlayerAtSeat(player_seat).bet(chips);
    pot_manager->addChips(chips);
}

void GameModel::takePotChipsToPlayer(seat_idx_t player_seat, chips_t chips) {
    pot_manager->takeChips(chips);
    seating_manager->getPlayerAtSeat(player_seat).win(chips);
}

void GameModel::applyBettingResult(seat_idx_t player_seat, math::BettingResult result) {
    if (result.is_target_bet_valid) {
        takePlayerChipsToPot(player_seat, result.bet_delta);
        betting_round->applyBettingResult(result);
    }
}

GameModel::GameModel(
    std::shared_ptr<EventBus> event_bus,
    std::unique_ptr<IDeck> deck,
    std::shared_ptr<PotManager> pot_manager,
    std::shared_ptr<SeatingManager> seating_manager,
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

void GameModel::startGame() {
    street = Street::IDLE;
    deck->shuffle();
    pot_manager->clearPot();
    betting_round->onGameStarted(seating_manager->getPlayerCount());
    event_bus->publish(Event::GameStarted{
        seating_manager->getBtnPosition()
    });
}

void GameModel::dealHoleCards() {
    for (Player& player : seating_manager->getPlayerList()) {
        cards_2_t hole_cards = {deck->deal(), deck->deal()};
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
    seat_idx_t sb_seat = seating_manager->getSbPosition();
    takePlayerChipsToPot(sb_seat, big_blind / 2);
    event_bus->publish(Event::BlindPosted{
        .seat = seating_manager->getSbPosition(),
        .amount = big_blind / 2,
        .is_big_blind = false
    });
    // Big Blind
    seat_idx_t bb_seat = seating_manager->getBbPosition();
    takePlayerChipsToPot(bb_seat, big_blind);
    betting_round->processForcedBlind(sb_seat, bb_seat, big_blind);
    event_bus->publish(Event::BlindPosted{
        .seat = seating_manager->getBbPosition(),
        .amount = big_blind,
        .is_big_blind = true
    });
}

void GameModel::enterPreflopStreet() {
    street = Street::PREFLOP;
    event_bus->publish(Event::StreetStarted{
        .street = street
    });
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
        seating_manager->getPreflopFirstPosition() :
        seating_manager->getPostflopFirstPosition();
    betting_round->onRoundStarted(first_actor_seat);
    event_bus->publish(Event::BettingRoundStarted{
        first_actor_seat
    });
}
} // namespace poker
