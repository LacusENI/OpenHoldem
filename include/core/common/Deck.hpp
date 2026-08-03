#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <random>

#include "Card.hpp"

namespace poker {

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
    static constexpr int CARDS_SIZE = 52;
    std::array<Card, CARDS_SIZE> cards;
    std::mt19937 rng;
    size_t next_card_index = 0;

public:
    Deck() {
        int i = 0;
        for (int s = MIN_SUIT; s <= MAX_SUIT; ++s) {
            for (int r = MIN_RANK; r <= MAX_RANK; ++r) {
                cards[i++] = Card(static_cast<Rank>(r), static_cast<Suit>(s));
            }
        }
        const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
    }

    /* 洗牌 */
    void shuffle() override {
        std::ranges::shuffle(cards, rng);
        next_card_index = 0;
    }

    /* 发牌 */
    Card deal() override {
        if (next_card_index >= CARDS_SIZE) {
            throw std::runtime_error("Deck::deal: index out of range");
        }
        return cards[next_card_index++];
    }
};
} // namespace poker
