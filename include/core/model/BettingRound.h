#pragma once

#include <optional>

#include "Event.hpp"
#include "GameTypes.hpp"

namespace poker {
class PlayerSet;
class SeatManager;

/* 未被跟注的多余下注：应退还给该玩家 */
struct UncalledBet {
    seat_t seat;
    chips_t amount;
};

/**
 * @brief 当前下注轮（a betting round）
 * @details 合并原 BettingManager（下注额/盲注/未跟注）与 ActionFlow（行动顺序/轮次生命周期）。
 *          两者是同一事物的两面——"轮到谁行动"由"已下注多少"（needsToAct(round_bet)）决定，
 *          拆开后需要反复在两者间传递 round_bet，故合并。
 */
class BettingRound {
private:
    PlayerSet& player_set;
    SeatManager& seat_manager;

    // --- 金额 ---
    chips_t big_blind = 10;
    chips_t round_bet = 0;
    chips_t min_raise_delta = 0;

    // --- 行动顺序 ---
    seat_t acting_player = INVALID_SEAT;
    seat_t last_aggressor = INVALID_SEAT;
    bool has_incomplete_raise = false;

    /* 记录已分类行动对行动顺序的影响（acted / 玩家状态 / 是否完整加注） */
    void recordAction(Action action);

public:
    BettingRound(PlayerSet& player_set, SeatManager& seat_manager)
        : player_set(player_set), seat_manager(seat_manager) {}
    ~BettingRound() = default;

    // ===== 金额 / 盲注 =====
    chips_t getPlayerBetAt(seat_t seat_idx) const;
    void setPlayerBetAt(seat_t seat_idx, chips_t player_bet);

    chips_t getBigBlind() const { return big_blind; }
    void setBigBlind(chips_t value) { big_blind = value; }
    chips_t getSmallBlind() const { return big_blind / 2; }

    chips_t getRoundBet() const { return round_bet; }
    void setRoundBet(chips_t value) { round_bet = value; }
    chips_t getMinRaiseDelta() const { return min_raise_delta; }
    void setMinRaiseDelta(chips_t value) { min_raise_delta = value; }
    chips_t getMinRaise() const { return round_bet + min_raise_delta; }

    /* 进入新一轮时清零各人下注与轮次金额（盲注在其后投入） */
    void resetBets();
    /* 执行小盲注，返回实际投入量（筹码不足时投入全部筹码） */
    chips_t applySmallBlind(seat_t sb_seat_idx);
    /* 执行大盲注，返回实际投入量（筹码不足时投入全部筹码） */
    chips_t applyBigBlind(seat_t bb_seat_idx);
    /* 本轮唯一最高下注者未被跟注的部分；没有则返回 nullopt */
    std::optional<UncalledBet> findUncalledBet() const;

    // ===== 行动顺序 / 轮次生命周期 =====
    seat_t getActingPlayer() const { return acting_player; }
    void setActingPlayer(seat_t player) { acting_player = player; }
    [[nodiscard]] bool hasActingPlayer() const { return acting_player != INVALID_SEAT; }
    /* 本轮最后一个激进者（下注/加注者）；无则 INVALID_SEAT */
    [[nodiscard]] seat_t getLastAggressor() const { return last_aggressor; }
    [[nodiscard]] bool hasIncompleteRaise() const { return has_incomplete_raise; }

    /* 本轮仍需行动的玩家数量 */
    [[nodiscard]] int needsToActCount() const;
    /* 下注轮是否结束（无人需要行动，或仅剩一名在局玩家） */
    [[nodiscard]] bool isFlowEnded() const;
    /* 该玩家本轮是否仍可加注 */
    [[nodiscard]] bool canPlayerRaise(seat_t seat) const;

    /* 移动行动者到下一位需要行动的玩家 */
    void moveActingPlayer();
    /* 初始化本轮起始行动者并清空 acted 标记（应在盲注投入之后调用） */
    void setupToStart(Stage stage);
    /* 处理当前行动者的一次行动：构建下注上下文、落子筹码与下注额、更新行动顺序；
       无效输入返回 nullopt，否则返回可供广播的 PlayerActed */
    std::optional<Event::PlayerActed> applyAction(const Command::SubmitAction& event);
};
} // namespace poker
