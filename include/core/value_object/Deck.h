#pragma once

#include <random>
#include <array>

#include "Card.h"

namespace holdem {

constexpr int CARDS_SIZE = 52;

/**
 * @brief 扑克牌组接口
 */
class IDeck {
public:
    virtual ~IDeck() = default;
    virtual Card deal() = 0;
    virtual void shuffle() = 0;
};

/**
 * @brief 扑克牌组
 */
class Deck final : public IDeck {
private:
    std::array<Card, CARDS_SIZE> cards;
    std::mt19937 rng;
    size_t next_card_index = 0;
public:
    Deck();

    /* 洗牌 */
    void shuffle() override;

    /* 发牌 */
    Card deal() override;
};
}
