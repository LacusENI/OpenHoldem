#ifndef OPENHOLDEM_DECK_H
#define OPENHOLDEM_DECK_H
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>
#include <stdexcept>

#include "Card.h"

namespace holdem {

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
class Deck : public IDeck {
private:
    std::vector<Card> cards;
    std::mt19937 rng;
    size_t next_card_index = 0;
public:
    Deck() {
        cards.reserve(52);
        for (int s = 0; s < 4; ++s) {
            for (int r = 2; r <= 14; ++r) {
                cards.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
            }
        }
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
    }

    /* 洗牌 */
    void shuffle() override {
        std::shuffle(cards.begin(), cards.end(), rng);
        next_card_index = 0;
    }

    /* 发牌 */
    Card deal() override {
        if (next_card_index >= cards.size()) {
            throw std::runtime_error("No more cards!");
        }
        return cards[next_card_index++];
    }
};
}



#endif //OPENHOLDEM_DECK_H