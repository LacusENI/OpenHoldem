#include "ChipsManager.h"

#include <algorithm>

#include "PlayerSet.h"
#include "PotManager.h"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
void ChipsManager::sortWinners(std::vector<seat_idx_t>& winners) {
    seat_idx_t ref_seat = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
    std::ranges::sort(winners, [&ref_seat, this](seat_idx_t seat_a, seat_idx_t seat_b) {
        return this->seat_manager->distanceFromSeatLeft(seat_a, ref_seat) <
               this->seat_manager->distanceFromSeatLeft(seat_b, ref_seat);
    });
}

chips_t ChipsManager::getPlayerChipsAt(seat_idx_t seat_idx) {
    return player_set->playerAt(seat_idx).chips;
}

void ChipsManager::setPlayerChipsAt(seat_idx_t seat_idx, chips_t player_chips) {
    player_set->playerAt(seat_idx).chips = player_chips;
}

void ChipsManager::moveChipsToPot(seat_idx_t player_seat, chips_t chips) {
    player_set->playerAt(player_seat).commitChips(chips);
    pot_manager->addChips(chips);
}

void ChipsManager::moveChipsToPlayer(seat_idx_t player_seat, chips_t chips) {
    pot_manager->takeChips(chips);
    player_set->playerAt(player_seat).winChips(chips);
}

void ChipsManager::awardWinners(const std::vector<seat_idx_t>& winners) {
    int winners_n = static_cast<int>(winners.size());
    auto awards = algo::calculate_awards(pot_manager->getPot(), winners_n);
    for (int i = 0; i < winners_n; i++) {
        seat_idx_t winner_seat = winners[i];
        chips_t award = awards[i];
        moveChipsToPlayer(winner_seat, award);
    }
}
} // namespace poker