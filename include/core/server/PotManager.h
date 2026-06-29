#pragma once
#include "GameTypes.h"

namespace poker {
/**
 * @brief 管理底池的类
 */
class PotManager {
private:
    chips_t pot = 0;

public:
    PotManager() = default;

    /* 获取底池数额 */
    chips_t getPot() const { return pot; }
    /* 向底池投入筹码 */
    void addChips(chips_t amount) { pot += amount; }
    /* 从底池拿取筹码 */
    void takeChips(chips_t amount) { pot -= amount; }
    /* 清空底池 */
    void clearPot() { pot = 0; }
};
} // namespace poker