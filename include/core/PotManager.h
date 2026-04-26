#pragma once
#include "GameTypes.h"

namespace holdem {
/**
 * @brief 管理底池的类
 */
class PotManager {
private:
    Stack pot = 0;
public:
    PotManager() = default;

    /* 获取底池数额 */
    Stack get() const { return pot; }
    /* 向底池投入筹码 */
    void addChipsToPot(Stack amount) { pot += amount; }
    /* 从底池拿取筹码 */
    void takeChips(Stack amount) { pot -= amount; }
    /* 清空底池 */
    void clearPot() { pot = 0; }
};
} // holdem