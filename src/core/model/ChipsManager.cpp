#include "ChipsManager.h"

#include <format>
#include <stdexcept>

#include "GameTypes.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
chips_t ChipsManager::getPlayerChipsAt(seat_t seat_idx) {
    return player_set.playerAt(seat_idx).getChips();
}

void ChipsManager::setPlayerChipsAt(seat_t seat_idx, chips_t player_chips) {
    player_set.playerAt(seat_idx).setChips(player_chips);
}

void ChipsManager::splitThePot() {
    std::vector<std::pair<seat_t, chips_t>> players_with_committed;
    for (seat_t player : player_set.getPlayingPlayers()) {
        chips_t committed = player_set.playerAt(player).getCommitted();
        players_with_committed.emplace_back(player, committed);
    }
    pots = load_side_pots(players_with_committed);
}

void ChipsManager::awardPotTo(seat_t winner) {
    // 汇总全部在局玩家的投入（含弃牌者的死钱），整池判给唯一存活玩家
    chips_t total = 0;
    for (seat_t player : player_set.getPlayingPlayers()) {
        total += player_set.playerAt(player).getCommitted();
    }
    clearPots();
    moveChipsToPlayer(winner, total);
}

void ChipsManager::moveChipsToPot(seat_t player_seat, chips_t chips) {
    Player& player = player_set.playerAt(player_seat);
    if (chips > player.getChips()) {
        throw std::invalid_argument(std::format("Player's chips is not enough"));
    }
    player.payChips(chips);
}

void ChipsManager::moveChipsToPlayer(seat_t player_seat, chips_t chips) {
    // TODO
    player_set.playerAt(player_seat).winChips(chips);
}

void ChipsManager::distributePot(chips_t pot_chips, std::vector<seat_t>& winners) {
    // 防御：无赢家可分（不应发生），直接返回避免除零
    if (winners.empty()) return;

    seat_manager.sortSeats(winners);
    int winners_n = static_cast<int>(winners.size());
    auto awards = algo::calculate_awards(pot_chips, winners_n);
    for (int i = 0; i < winners_n; i++) {
        seat_t winner_seat = winners[i];
        chips_t award = awards[i];
        moveChipsToPlayer(winner_seat, award);
    }
}
} // namespace poker