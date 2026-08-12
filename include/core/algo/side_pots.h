#include <set>
#include <utility>
#include <vector>

#include "GameTypes.hpp"

namespace poker {
struct Pot {
    chips_t chips;
    chips_t level;
    chips_t contribution;
    std::vector<seat_t> contributors;
};

inline std::vector<Pot> load_side_pots(
    std::vector<std::pair<seat_t, chips_t>> players_with_committed) {
    std::set<chips_t> levels;
    for (auto [_, committed] : players_with_committed) {
        if (committed > 0) {
            levels.insert(committed);
        }
    }
    std::vector<Pot> pots;
    for (chips_t level : levels) {
        pots.emplace_back();
        pots.back().level = level;
    }
    chips_t last_level = 0;
    for (Pot& pot : pots) {
        pot.contribution = pot.level - last_level;
        last_level = pot.level;
    }
    for (Pot& pot : pots) {
        for (auto [player, committed] : players_with_committed)
            if (committed >= pot.level) pot.contributors.push_back(player);
        pot.chips = pot.contributors.size() * pot.contribution;
    }
    return pots;
}
} // namespace poker