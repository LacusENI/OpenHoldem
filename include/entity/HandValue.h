#pragma once

#include <vector>

#include "Card.h"

namespace holdem {

/* 牌型 */
enum class HandType {
    HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT,
    FLUSH, FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH
};

namespace utils {
std::string castHandTypeToString(HandType hand_type);
}

/**
 * @brief 表示一手牌的强度和比较逻辑的类型
 */
class HandValue {
private:
    /**
     * @brief 表示手牌强度的分值
     * @details 采用 32 位整数编码, 整数越大，牌越大, 格式为：0xTPSKKK,
     * - T: HandType (4 bits)       牌型等级(0-9)
     * - P: Primary Rank (4 bits)   主牌点数(如葫芦的三条的点数)
     * - S: Secondary Rank (4 bits) 次高牌点数(如葫芦的对子的点数）
     * - K：Kickers (4 bits each)   踢脚牌点数(按从大到小排序)
     * - 按照比较顺序排列，不足的位置设置为零
     * - 例如: 葫芦: PS000，两条: PKKK0，两对：PSK00
     * @details 举例:
     * - 三条7，两对2的葫芦: 0x672000 (6表示葫芦)
     * - A, Q, 9, 7, 2的高牌: 0x0EC972 (0表示高牌, E表示Ace, C表示Queen)
     * - 两对9和5: 0x295000 (2表示两对)
     */
    uint32_t raw_value = 0;
public:
    HandValue() = default;
    explicit HandValue(int raw_value) : raw_value(raw_value) {}

    /* 获取此手牌分值对应的手牌类型 */
    [[nodiscard]] HandType getHandType() const {
        return static_cast<HandType>(raw_value >> 20);
    }

    /**
     * @brief 获取此手牌分值对应的主牌/次高牌/踢脚牌的点数,
     * 按参与比较的优先级排序
     * @param index 第X张计分牌的点数 (0为主牌，1为次高牌，2-4为踢脚牌)
     * @return 点数
     */
    [[nodiscard]] Rank getRank(const int index) const {
        return static_cast<Rank>((raw_value >> (16 - 4 * index)) & 0xF);
    }

    bool operator==(const HandValue &other) const {
        return raw_value == other.raw_value;
    }

    bool operator>(const HandValue &other) const {
        return raw_value > other.raw_value;
    }

    bool operator<(const HandValue &other) const {
        return raw_value < other.raw_value;
    }

    /**
     * 生成 HandValue 工厂方法
     * @param hand_type 手牌牌型
     * @param ranks 按比较优先级排列的点数 (不重复)
     * @return HandValue
     */
    HandValue static build(HandType hand_type, std::vector<Rank> ranks);
};
}
