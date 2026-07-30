#include "PlayerSet.h"

#include <algorithm>

namespace poker {
int PlayerSet::getActivePlayerCount() {
    return std::ranges::count_if(players, [](const Player& p) { return p.isActive(); });
}
int PlayerSet::getActionablePlayerCount() {
    return std::ranges::count_if(players, [](const Player& p) { return p.canAct(); });
}
} // namespace poker