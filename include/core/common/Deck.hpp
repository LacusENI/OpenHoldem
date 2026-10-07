#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
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
 * @details 支持指定随机种子以便复现；默认使用系统时钟种子。
 */
class Deck final : public IDeck {
private:
    static constexpr int CARDS_SIZE = 52;
    std::array<Card, CARDS_SIZE> cards;
    std::mt19937 rng;
    std::uint32_t seed;
    size_t next_card_index = 0;

    void fillCards() {
        int i = 0;
        for (int s = MIN_SUIT; s <= MAX_SUIT; ++s) {
            for (int r = MIN_RANK; r <= MAX_RANK; ++r) {
                cards[i++] = Card(static_cast<Rank>(r), static_cast<Suit>(s));
            }
        }
    }

public:
    /* 系统时钟随机种子 */
    static std::uint32_t randomSeed() {
        return static_cast<std::uint32_t>(
            std::chrono::system_clock::now().time_since_epoch().count());
    }

    /* 随机种子构造 */
    Deck() : Deck(randomSeed()) {}
    /* 指定种子构造（可复现） */
    explicit Deck(std::uint32_t seed_value) : seed(seed_value) {
        fillCards();
        rng.seed(seed);
    }

    /* 本牌组使用的种子 */
    std::uint32_t getSeed() const { return seed; }

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
