#include <vector>
#include <algorithm>
#include "HandEvaluator.h"

namespace holdem::internal {

std::vector<Hand5> getCombinations(const Hand7& hand7) {
    std::vector<Hand5> combinations;

    std::vector<bool> mask = {false, false, true, true, true, true, true};

    do {
        Hand5 combo;
        int combo_i = 0;
        for (int i = 0; i < mask.size(); i++) {
            if (mask[i]) { combo[combo_i++] = hand7[i]; }
        }
        combinations.push_back(combo);
    } while (std::next_permutation(mask.begin(), mask.end()));

    return combinations;
}

std::vector<RankCount> getCounts(const Hand5& hand5) {
    std::array<int, 15> buckets = {};
    for (Card card : hand5) {
        buckets[static_cast<int>(card.rank)]++;
    }
    std::vector<RankCount> counts;
    for (int i = 0; i < buckets.size(); i++) {
        if (buckets[i] > 0) {
            Rank rank = static_cast<Rank>(i);
            counts.emplace_back(rank, buckets[i]);
        }
    }
    std::sort(counts.begin(), counts.end(), std::greater<>());
    return counts;
}
}