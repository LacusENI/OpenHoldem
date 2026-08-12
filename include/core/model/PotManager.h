#pragma once

#include <vector>

#include "GameTypes.hpp"
#include "side_pots.h"

namespace poker {
/**
 * @brief 管理底池的类
 */
class PotManager {
private:
    chips_t pot = 0;
    std::vector<Pot> pots;

public:
    PotManager() = default;

    /* 获取底池数额 */
    chips_t getPot() const { return pot; }
    const std::vector<Pot>& getPots() const { return pots; }
    void setPots(const std::vector<Pot>& pots) { this->pots = pots; }

    /* 向底池投入筹码 */
    void addChips(chips_t amount) { pot += amount; }
    /* 从底池拿取筹码 */
    void takeChips(chips_t amount) { pot -= amount; }
    /* 清空底池 */
    void clearPot() { pot = 0; }
};
} // namespace poker