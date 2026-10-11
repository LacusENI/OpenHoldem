# TESTING · 测试策略

> 本项目的测试原则、运行方式、组织约定与覆盖重点。
> 关于"**不要自作主张地添加/修改测试**"的协作规则见 `AGENTS.md`。

## 1. 原则

- 项目处于**快速迭代 / 重构期**：测试**酌情添加**——追求"稳住关键不变量 + 修 bug 有回归"，**不追求覆盖率数字**。
- **什么时候必须加**：
  - 修一个 bug → 先加能**复现**它的回归测试（先红后绿）。
  - 触及**筹码守恒 / all-in / 边界**等核心不变量时。
  - 被 `docs/DECISIONS.md` / `docs/GLOSSARY.md` 明文规定的行为。
- **什么时候先不加**：纯重命名 / 搬移 / 结构整理（除非能证明等价）；**尚未定稿**的设计（如实验性下注模型）。
- **不要**为凑覆盖率写测试；**不要**写"复述实现"的测试。

## 2. 如何运行

```bash
# 配置（含测试）
cmake --preset debug-with-tests
# 构建
cmake --build --preset debug
# 全量
ctest --preset test                       # 或：ctest --test-dir cmake-build-debug
# 失败详情
ctest --test-dir cmake-build-debug --output-on-failure
# 单个套件/用例（GTest 过滤）
./cmake-build-debug/poker_test --gtest_filter='BettingRoundTest.*'
```

核心改动后应**全绿**。应用层目标暂不参与测试（见 `docs/ROADMAP.md`）。

## 3. 组织约定

- **一个模块一个测试文件**：`test/<模块>_test.cpp`（对应 `include/core/**`）。
- 目前源码中约 **81** 个 `TEST/TEST_F`（以 `ctest` 输出为准）。
- 需要替身时用 `test/include/MockDeck.hpp`（`MockDeck` / `MockEmptyDeck`，实现 `IDeck`；配合 `Deck` 可播种）。
- 命名：套件 `<模块>Test` + **行为化**用例名（说清"在什么条件下应发生什么"）。

## 4. 测试分层

- **单元**：纯算法 / 值类型 —— `eval_hand`、`side_pots`、`Card`、`GameTypes`。
- **领域**：`Player`（筹码 / all-in 唯一入口）、`PotManager`、`BettingRound`、`SeatManager`、`CardsManager`、`GameModel`。
- **不变量 / 集成**：`test/invariants_test.cpp` —— 多手（可播种）随机，断言全局不变量。

## 5. 覆盖重点（不变量）

- **筹码守恒**：任一时刻，玩家筹码 + 各池筹码 = 初始总量。
- **all-in / 边池**：分层正确；主池 / 边池**分配守恒**。
- **未跟注退还**：每轮结束退还，且底池不含未跟注部分。
- **无人竞争**：`alive == 1` 时整池判给唯一存活者。
- **摊牌顺序**：TDA 16 / 17（见 `docs/GLOSSARY.md`）。
- **结算**：`committed` 清零。
- **重开下注**：见 `docs/design/reopen-betting.md` §9 检查清单（**实现待做**）。

## 6. 复现

- 随机相关缺陷**固定种子**（`Deck(seed)`），并把**种子**写进报告 / 提交说明。
- demo 也可带种子：`./cmake-build-debug/poker 12345`。

## 7. 待办

- fuzz / 长跑不变量测试（随机多手，见 `docs/ROADMAP.md` P2）。
- 把 `GameController::decideAfterBettingRound()` 提为**可单测的纯函数**并补用例。
- 重开规则回归测试（`docs/ROADMAP.md` P0）。
