#include "BettingRound.h"

#include <algorithm>

#include "Player.hpp"
#include "PlayerSet.hpp"
#include "SeatManager.h"
#include "chips_calc.h"

namespace poker {
// ===== 金额 / 盲注 =====
chips_t BettingRound::getPlayerBetAt(seat_t seat_idx) const {
    return player_set->playerAt(seat_idx).getBet();
}

void BettingRound::setPlayerBetAt(seat_t seat_idx, chips_t player_bet) {
    player_set->playerAt(seat_idx).setBet(player_bet);
}

void BettingRound::resetBets() {
    round_bet = 0;
    min_raise_delta = 0;
    for (seat_t seat_idx : player_set->getPlayerSeats()) {
        setPlayerBetAt(seat_idx, 0);
    }
}

chips_t BettingRound::applySmallBlind(seat_t sb_seat_idx) {
    chips_t actual = std::min(getSmallBlind(), player_set->playerAt(sb_seat_idx).getChips());
    setPlayerBetAt(sb_seat_idx, actual);
    round_bet = std::max(round_bet, actual);
    min_raise_delta = std::max(min_raise_delta, actual);
    return actual;
}

chips_t BettingRound::applyBigBlind(seat_t bb_seat_idx) {
    chips_t actual = std::min(getBigBlind(), player_set->playerAt(bb_seat_idx).getChips());
    setPlayerBetAt(bb_seat_idx, actual);
    // 大盲名义额作为本轮的 bring-in，即使大盲玩家筹码不足
    round_bet = std::max(round_bet, big_blind);
    min_raise_delta = std::max(min_raise_delta, big_blind);
    return actual;
}

std::optional<UncalledBet> BettingRound::findUncalledBet() const {
    chips_t max1 = 0;  // 最高注
    chips_t max2 = 0;  // 次高注（严格小于 max1）
    int count_max = 0; // 达到 max1 的人数
    seat_t leader = INVALID_SEAT;

    for (seat_t seat = 0; seat < player_set->playerCount(); ++seat) {
        const Player& player = player_set->playerAt(seat);
        if (player.isFolded() || player.isOut()) continue; // 弃牌/出局者不计
        const chips_t bet = player.getBet();
        if (bet > max1) {
            max2 = max1;
            max1 = bet;
            count_max = 1;
            leader = seat;
        } else if (bet == max1) {
            ++count_max;
        } else if (bet > max2) {
            max2 = bet;
        }
    }

    if (leader != INVALID_SEAT && count_max == 1 && max1 > max2) {
        return UncalledBet{leader, max1 - max2};
    }
    return std::nullopt;
}

// ===== 行动顺序 / 轮次生命周期 =====
int BettingRound::needsToActCount() const {
    int count = 0;
    for (const Player& player : *player_set) {
        if (player.needsToAct(round_bet)) ++count;
    }
    return count;
}

bool BettingRound::isFlowEnded() const {
    return needsToActCount() == 0 || player_set->getAlivePlayerCount() == 1;
}

bool BettingRound::canPlayerRaise(seat_t seat) const {
    const Player& player = player_set->playerAt(seat);
    return !(player.isActed() && has_incomplete_raise);
}

void BettingRound::moveActingPlayer() {
    acting_player = seat_manager->nextSeatNeedsToAct(acting_player, round_bet);
}

void BettingRound::setupToStart(Stage stage) {
    if (isPreflop(stage)) {
        acting_player = seat_manager->getUtgSeat();
    } else if (isPostflop(stage)) {
        acting_player = seat_manager->nextActiveSeat(seat_manager->getBtnSeat());
    } else {
        // 非下注阶段（IDLE/SHOWDOWN/SETTLEMENT）没有行动者
        acting_player = INVALID_SEAT;
    }
    for (Player& player : *player_set) {
        player.setNotActed();
    }
    last_aggressor = INVALID_SEAT;
    has_incomplete_raise = false;
}

void BettingRound::recordAction(Action action) {
    if (action == Action::Raise || action == Action::Bet || action == Action::ShortAllIn) {
        last_aggressor = acting_player;
        has_incomplete_raise = false;
    }
    player_set->playerAt(acting_player).setActed();
    if (action == Action::Fold) {
        player_set->playerAt(acting_player).fold();
    }
    if (action == Action::ShortAllIn) {
        has_incomplete_raise = true;
    }
}

std::optional<Event::PlayerActed> BettingRound::applyAction(const Event::PlayerAction& event) {
    const seat_t player_seat = acting_player;
    if (!hasActingPlayer()) return std::nullopt;

    if (event.is_fold) {
        recordAction(Action::Fold);
        return Event::PlayerActed{
            .player = player_seat, .bet = 0, .action = Action::Fold, .is_all_in = false};
    }

    const Player& player = player_set->playerAt(player_seat);
    algo::BettingContext context = {.player_chips = player.getChips(),
                                    .player_bet = player.getBet(),
                                    .round_bet = round_bet,
                                    .min_raise_delta = min_raise_delta,
                                    .can_raise = canPlayerRaise(player_seat)};
    algo::BettingInput input = {.target_bet = event.target_bet, .is_fold = event.is_fold};
    algo::BettingResult result = algo::process_betting(context, input);
    if (!result.isValid()) return std::nullopt;

    player_set->playerAt(player_seat).commit(result.player_bet);
    round_bet = result.round_bet;
    min_raise_delta = result.min_raise_delta;
    recordAction(result.action);

    return Event::PlayerActed{.player = player_seat,
                              .bet = result.player_bet,
                              .action = result.action,
                              .is_all_in = player_set->playerAt(player_seat).isAllIn()};
}
} // namespace poker
