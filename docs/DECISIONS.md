# DECISIONS

> 架构决策记录（ADR）。**append-only**：不改写历史条目；被推翻时把旧条目标为 `Superseded by NNNN` 并新增一条。
> **改设计前先查这里有没有冲突。**
> 状态：`Accepted`（已定并已实现）· `Proposed`（待定）· `Superseded`（被取代）。

## 决策原则（贯穿全项目）

- **P1 显式优于隐式**：合法则接受；非法则**拒绝 / 断言**，不静默改写。
- **P2 早失败**：关键路径加断言，让问题在源头暴露。
- **P3 组合根集中装配**：由一个根对象持有全部组件、以引用注入，避免裸指针与分散构造。
- **P4 核心纯净**：`core` 不依赖网络/序列化/表现；需要外部能力时定义**端口**，由应用层实现。
- **P5 单一真源**：同一事实只在一处权威，别处只链接。

## 索引

| # | 标题 | 状态 | 日期 |
|---|---|---|---|
| 0001 | all-in 是玩家状态而非动作意图 | Accepted | 2026-10-07 |
| 0002 | `Action` 枚举 CamelCase，删除 `ALL_IN_CALL`/`ALL_IN_RAISE` | Accepted | 2026-10-07 |
| 0003 | 未跟注筹码在每轮结束退还 | Accepted | 2026-10-06 |
| 0004 | 无人竞争时整池直接判给唯一存活者 | Accepted | 2026-10-06 |
| 0005 | 摊牌顺序（TDA 16/17），暂不 muck | Accepted | 2026-10-07 |
| 0006 | 结算后清零 `committed` | Accepted | 2026-10-07 |
| 0007 | 组合根 `GameSession` + 引用注入 | Accepted | 2026-10-07 |
| 0008 | `GameModel` 返回事件，`GameController` 发布（与 `EventBus` 解耦） | Accepted | 2026-10-07 |
| 0009 | `GameController` 状态机化 | Accepted | 2026-10-07 |
| 0010 | `ChipsManager` 更名 `PotManager`，删除筹码增减薄包装 | Accepted | 2026-10-07 |
| 0011 | 显式出站端口 `ClientGateway` 取代 `ServerIoAdapter` | Proposed | 2026-10-10 |
| 0012 | 实验：显式玩家行动模型 `NewPlayerAction` / `betting.h` | Proposed | 2026-10-10 |

---

## 0001 · all-in 是玩家状态而非动作意图

- **状态**：Accepted（2026-10-07，commit `53de4b3`）
- **背景**：早期把 all-in 当作一种动作种类（`ALL_IN_CALL`/`ALL_IN_RAISE`），导致"动作"与"玩家状态"混为一谈。
- **决策**：all-in 是**玩家状态**，由 `Player` 在 `payChips`(扣到 0) / `winChips` / `refund` 时维护 `ALL_IN` / `ACTIVE`。
- **后果**：动作只表达意图；合法性由结果标志（是否全下/不足额）体现；`Player` 成为筹码的唯一入口。

## 0002 · `Action` 枚举 CamelCase，删除 `ALL_IN_CALL`/`ALL_IN_RAISE`

- **状态**：Accepted（2026-10-07，commit `53de4b3`）
- **决策**：`enum class Action { Void, Fold, Check, Call, Bet, Raise, Undercall, ShortAllIn }`；命名为 CamelCase；删除 `ALL_IN_CALL`/`ALL_IN_RAISE`。
- **后果**：需同步修改所有使用旧名的地方（应用层因此曾编译失败，属预期）。

## 0003 · 未跟注筹码在每轮结束退还

- **状态**：Accepted（2026-10-06，commit `5ce14c7`）
- **背景**：下注轮存在"多下而无人跟"的未跟注筹码。
- **决策**：**每次下注轮结束**时，把未跟注部分退还给下注者，再进入下一阶段。
- **后果**：事件 `UncalledBetReturned`；底池计算不含未跟注部分。

## 0004 · 无人竞争时整池直接判给唯一存活者

- **状态**：Accepted（2026-10-06，commit `a0a4eb5`）
- **背景**：只剩一人未弃牌时，若按常规摊牌/切池会出错。
- **决策**：`alive == 1` 时，**整池直接判给唯一存活者**，**不切分底池**、不摊牌。
- **后果**：需要显式分支，避免"空池/无人认领"。

## 0005 · 摊牌顺序（TDA 16/17），暂不 muck

- **状态**：Accepted（2026-10-07，commit `9799684`）
- **决策**：
  - 任一人 all-in → **一次全亮**（TDA 16）。
  - 否则从**河牌轮最后激进者**（无则按钮左侧）起，顺时针逐个亮牌（TDA 17）。
  - **暂不实现 muck**。
- **后果**：修复了"全下玩家漏牌"的问题。

## 0006 · 结算后清零 `committed`

- **状态**：Accepted（2026-10-07，commit `6f35547`）
- **背景**：本手累计投入 `committed` 若不清零，会污染下一手。
- **决策**：一手结算后清零 `committed`；并补不变量测试。
- **后果**：多手连续进行时状态干净。

## 0007 · 组合根 `GameSession` + 引用注入

- **状态**：Accepted（2026-10-07，commit `2847ae9`）
- **背景**：组件分散构造、裸指针传递，生命周期与依赖关系不清。
- **决策**：引入 `GameSession` 作为**组合根**，持有并装配全部组件；依赖由指针改为**引用**注入。
- **后果**：构造与所有权集中；测试可注入 mock 依赖。

## 0008 · `GameModel` 返回事件，`GameController` 发布

- **状态**：Accepted（2026-10-07，commit `4b9648d`）
- **背景**：领域模型直接依赖 `EventBus` 发布，耦合且难测。
- **决策**：`GameModel` 的方法**返回事件**，由 `GameController` 负责发布到 `EventBus`。
- **后果**：`GameModel` 纯化、可单测；`GameController` 成为事件发布的唯一出口（也为日后的显式出站端口铺路，见 0011）。

## 0009 · `GameController` 状态机化

- **状态**：Accepted（2026-10-07，commit `3aab9fd`）
- **决策**：把"决策"与"进入阶段"分离：`decideAfterBettingRound()` 纯决策；`enterStage`/`enter*` 执行副作用。
- **后果**：决策逻辑可独立测试；`BettingRoundStarted` 事件携带 `stage`（commit `3ee48f1`）。

## 0010 · `ChipsManager` 更名 `PotManager`，删除筹码增减薄包装

- **状态**：Accepted（2026-10-07，commit `7fe7c82`）
- **决策**：`ChipsManager` → `PotManager`（它管理的是底池，不是筹码的加减）；删除 `moveChipsToPot`/`moveChipsToPlayer` 这类薄包装。
- **后果**：筹码增减归 `Player` 唯一入口，底池只做池子划分/归属。

## 0011 · 显式出站端口 `ClientGateway` 取代 `ServerIoAdapter`（Proposed）

- **状态**：**Proposed**（2026-10-10，待讨论）——详见 `docs/design/client-gateway.md`。
- **背景**：`ServerIoAdapter` 通过"订阅 `EventBus` 的 12 个事件 + 各自决定广播/单传"来实现出站，被评估为：路由隐藏、靠订阅表易漏、私密信息（底牌）在本地总线上广播、覆盖面漂移。
- **决策（拟）**：`core` 定义出站端口
  ```cpp
  class ClientGateway {
  public:
      virtual ~ClientGateway() = default;
      virtual void broadcast(const GameEvent& event) = 0;            // 广播
      virtual void sendTo(seat_t seat, const GameEvent& event) = 0;  // 单传
  };
  ```
  `GameController` 在事件产生处**显式**调用；`EventBus` 退化为**入站**通道；应用层实现该端口（网络/控制台/测试）。
- **待定点**：是否统一出站为唯一通道（EventBus 仅入站）；`seat ↔ connection` 映射；端口位置与分层；客户端侧对称端口。
- **后果（预期）**：路由显式、可测（`RecordingGateway`）、删除 `ServerIoAdapter`。

## 0012 · 实验：显式玩家行动模型 `NewPlayerAction` / `betting.h`（Proposed）

- **状态**：**Proposed / 待定**（2026-10-07 起，2026-10-10 暂缓）
- **背景**：希望把玩家动作从"目标额协议"改为**意图**（`Fold | Check | Call | Bet | Raise | AllIn`），与"结果"共用词汇；限制推导结果（全下/不足额/是否重开）用**结果标志**表达。
- **已定子决策**：取消"Bet/Raise 恰好全下即视为合法"的隐式豁免——`Bet`/`Raise` 必须是合法完整下注/加注，**短全下/全下只能用 `AllIn`**。
- **现状**：早期实现 `new_chips_calc` 已从构建中移除；新草稿落在 `include/core/algo/betting.h`（`poker::betting`，**未纳入构建**，`process()` 未实现）。
- **待定**：是否替换现有 `chips_calc`；是否同时把真实 `Action`/`PlayerAction` 改为意图协议。
