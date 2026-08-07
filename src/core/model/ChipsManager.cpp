#include "ChipsManager.h"

#include "PlayerSet.hpp"
#include "PotManager.h"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
chips_t ChipsManager::getPlayerChipsAt(seat_t seat_idx) {
    return player_set->playerAt(seat_idx).getChips();
}

void ChipsManager::setPlayerChipsAt(seat_t seat_idx, chips_t player_chips) {
    player_set->playerAt(seat_idx).setChips(player_chips);
}

void ChipsManager::moveChipsToPot(seat_t player_seat, chips_t chips) {
    player_set->playerAt(player_seat).commitChips(chips);
    pot_manager->addChips(chips);
}

void ChipsManager::moveChipsToPlayer(seat_t player_seat, chips_t chips) {
    pot_manager->takeChips(chips);
    player_set->playerAt(player_seat).winChips(chips);
}

void ChipsManager::distributePot(std::vector<seat_t>& winners) {
    seat_manager->sortSeats(winners);
    int winners_n = static_cast<int>(winners.size());
    auto awards = algo::calculate_awards(pot_manager->getPot(), winners_n);
    for (int i = 0; i < winners_n; i++) {
        seat_t winner_seat = winners[i];
        chips_t award = awards[i];
        moveChipsToPlayer(winner_seat, award);
    }
}
} // namespace poker