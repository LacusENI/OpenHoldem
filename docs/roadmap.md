# ROADMAP

> 路线图与当前待办

## 当前聚焦

**`demo-v9` 重构期：补完核心业务逻辑**（`include/core/**` + `src/core/**`）。
应用层（网络/序列化/表现）**暂缓**，其编译失败属预期。

## 待办（按优先级）

### P0 · 核心正确性

- [ ] **累计 short all-in 是否重开下注**：当前 `BettingRound` 用单个 `has_incomplete_raise` 布尔，无法正确处理"多个短全下累计达到完整加注额"的重开规则。
  - 方案：引入 `full_level` + 逐玩家 `acted` 判定。
  - 详见 `docs/design/reopen-betting.md`：规则 · 易错例子 · 伪代码 · 三种等价算法。
  - 需补回归测试。
- [ ] **手牌 / 会话生命周期语义**：区分"一手（hand）"与"一局/赛事（session）"；补 `HandStarted` / `HandEnded` 语义。

### P1 · 结构收敛（低风险整理）

- [ ] `ClientController` / `core/interface` 适配器**去裸指针**。
- [ ] 统一"双状态枚举"（`Player::State` 与 `PlayerStatus`）。
- [ ] `seat` 类型 / `optional` 化收敛。
- [ ] `const` 与命名统一。

### P2 · 测试加固

- [ ] **fuzz / 不变量测试**：随机若干手，断言筹码守恒、状态合法（`Deck` 已可播种）。
- [ ] 把 `GameController::decideAfterBettingRound()` 提为**可单测的纯函数**并补用例。

### P3 · 应用层（"打通局域网联机"）

- [ ] **显式出站端口 `ClientGateway`**：`GameController` 显式 `broadcast/sendTo`；删除 `ServerIoAdapter`；`EventBus` 仅入站。见 `docs/design/client-gateway.md`（`DECISIONS.md` 0011，Proposed）。
- [ ] 序列化（`EventSerializer`）与最新 `Event` 字段对齐。
- [ ] **结构化事件日志（JSON 化）**：把 `logging::EventLogger` 与 demo 中手写的 `printEvent` 重载，统一为"序列化 `GameEvent` 再打印"（复用 `serializeEvent`），消除逐事件格式串、避免字段漂移。
  - 前置：`EventSerializer` 对齐（上一项）且序列化对 demo 可用——目前它在 `application` 且编译不过，demo 只链 `poker_core`。
  - 建议随出站端口 `ClientGateway` 一并做成 `LoggingGateway` / sink，而非单独重订阅总线；泛型订阅需一张与 `GameEvent` variant 同源的类型列表。
  - 边界：`presentation`（`ConsoleView`/`Formatter`）是给人看的界面，**保留专用格式化**，不 JSON 化。
  - 性能：事件频率低，nlohmann/json 开销可忽略；只需对日志按级别短路，避免热路径无条件序列化。
- [ ] 修复/重写 `src/server_main.cpp`、`src/client_main.cpp`（现为旧代码）。
- [ ] 修复 `presentation`（`ConsoleView`/`Formatter`，旧 `Action` 命名）。
- [ ] CMake 选项化（核心 / 应用 / 测试目标分离）+ CI。

### P4 · 功能

- [ ] **简单 AI 对手**（随机/半随机策略），让单机可玩。
- [ ] **控制台 UI 增强**（彩色输出 / 桌面排版）。
- [ ] **锦标赛模式**（盲注递增、淘汰、多桌）。
- [ ] **图形界面**（Qt 或 SFML）。

### P5 · 待定实验 / 未来方向（暂缓）

- [ ] `betting.h` / 显式意图下注模型定稿（见 `DECISIONS.md` 0012）。
- [ ] **状态组合化**（把状态拆成内聚组件 + 函数式核心）：见 `docs/design/state-composition.md`。**暂缓**，等开始搭客户端 / 需要两端复用时启动。

## 已完成里程碑

- 阶段 0：低风险快修（`GlobalLogger` no-op、include 补齐、`Card`/`next_stage` 防御、`evalPatternHandType` constexpr、死代码清理）。
- 无人竞争整池结算（`a0a4eb5`）。
- 未跟注退还 + 拆分 `GameController`（`5ce14c7`）；摊牌顺序（`9799684`）。
- `BettingRound` 合并（`53b0405`）；`applyAction -> optional<PlayerActed>`（`3c95bfa`）；`Action` CamelCase + all-in 归 `Player`（`53de4b3`）。
- `GameSession` 组合根 + 引用注入（`2847ae9`）；结算清零 `committed` + 不变量测试（`6f35547`）；`Deck` 可播种 + demo 种子（`dd5f5d1`）。
- `GameController` 状态机化（`3aab9fd`）；`BettingRoundStarted` 携带 `stage`（`3ee48f1`）。
- `PotManager` 改名 + 删薄包装（`7fe7c82`）；`GameModel` 与 `EventBus` 解耦（`4b9648d`）；关键路径加 `assert`（`c7458c9`）。
- 实验 `NewPlayerAction` + `new_chips_calc` 起步（`a0ea832`），随后暂缓（`2dc5602`）。
