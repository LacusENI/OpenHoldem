#pragma once

#include "GameTypes.hpp"
#include "PlayerSet.hpp"

namespace poker {
/**
 * @brief 玩家与座位管理类
 */
class SeatManager {
private:
    PlayerSet* player_set;
    seat_t btn_seat = 0;

    /* 本手牌是否只有两名在局玩家（单挑） */
    [[nodiscard]] bool isHeadsUp() const { return player_set->getAlivePlayerCount() == 2; }

public:
    SeatManager(PlayerSet* player_set) : player_set(player_set) {}
    ~SeatManager() = default;

    int playerCount() const { return player_set->playerCount(); }
    seat_t getBtnSeat() const { return btn_seat; }
    void setBtnSeat(seat_t new_btn_position) { btn_seat = new_btn_position; }
    /* 按钮移至下一个仍在局的玩家 */
    void advanceBtnSeat() { btn_seat = nextAliveSeat(btn_seat); }

    /* 寻找当前座位的下一个座位 */
    seat_t nextSeat(seat_t position) const { return (position + 1) % playerCount(); }
    /* 寻找当前座位的下一个最近的活跃玩家的座位 */
    seat_t nextActiveSeat(seat_t current) const;
    /* 寻找当前座位的下一个最近的留在牌局的玩家的座位 */
    seat_t nextAliveSeat(seat_t current) const;

    /* 小盲注的位置（跳过已出局玩家；单挑时按钮位本人即小盲） */
    seat_t getSbSeat() const {
        return isHeadsUp() ? btn_seat : nextAliveSeat(btn_seat);
    }
    /* 大盲注的位置（单挑时为按钮的下一位） */
    seat_t getBbSeat() const {
        return isHeadsUp() ? nextAliveSeat(btn_seat) : nextAliveSeat(getSbSeat());
    }
    /* 枪口位的位置（单挑时由按钮/小盲先行动） */
    seat_t getUtgSeat() const {
        return isHeadsUp() ? btn_seat : nextAliveSeat(getBbSeat());
    }

    int distanceFromSeatLeft(seat_t seat, seat_t ref_seat);
    void sortSeats(std::vector<seat_t>& seats);
};
} // namespace poker