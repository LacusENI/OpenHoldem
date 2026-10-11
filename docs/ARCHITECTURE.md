# ARCHITECTURE

> OpenHoldem（`refactor/v9` / `demo-v9`）架构地图。
> 本文只做"地图"：分层、模块职责、依赖方向、数据流、不变量、构建目标。
> 决策的**理由**见 `docs/DECISIONS.md`；待办见 `docs/ROADMAP.md`。

## 1. 总览与分层

```
┌───────────────────────────────────────────────┐
│ application（应用层）                           │
│   network / serialization / logging /          │
│   presentation  +  src/*_main.cpp              │
└───────────────▲───────────────┬───────────────┘
                │ 依赖 core      │ 实现 core 定义的端口
┌───────────────┴───────────────┴───────────────┐
│ core（核心领域，纯逻辑，不依赖网络/序列化）        │
│   common · event · model · server · client ·   │
│   algo · interface                             │
└────────────────────────────────────────────────┘
                        ▲
                        │
                    test（GoogleTest）
```

**核心规则**：依赖方向单向，`core` **不得**依赖 `application` 的任何东西（网络 / 序列化 / 表现 / 日志实现）。需要外部能力时由 `core` 定义**端口**（接口），由应用层实现（依赖倒置）。

## 2. 目录职责

| 目录 | 职责 | 关键内容 |
|---|---|---|
| `include/core/common` | 基础值类型 | `Card` `Deck` `GameTypes`(`Stage`/`Action`/`chips_t`/`seat_t`) `HandValue` `Player` `PlayerSet` |
| `include/core/event` | 事件与总线 | `Event.hpp`(`Event::*` 与 `NetworkEvent::*` → `GameEvent` 变体)、`EventBus` |
| `include/core/model` | 领域模型 | `BettingRound` `CardsManager` `PotManager` `SeatManager` |
| `include/core/server` | 服务端领域 | `GameSession`(组合根) `GameModel` `GameController` |
| `include/core/client` | 客户端领域 | `ClientController` `ClientModel` `ClientView` |
| `include/core/algo` | 纯算法 | `eval_hand` `side_pots` `chips_calc`（+ 草稿 `betting.h`，未构建） |
| `include/core/interface` | 端口/适配器 | `ServerIoAdapter` `ClientIoAdapter` `GlobalLogger` |
| `include/logging` | 日志 | `EventLogger` |
| `include/network` | 网络(SFML) | `ServerNetwork` `ClientNetwork` `MockNetwork` |
| `include/serialization` | 序列化(nlohmann/json) | `EventSerializer` `GeneralSerializer` |
| `include/presentation` | 表现 | `ConsoleView` `Formatter` |

> `src/**` 与 `include/**` 一一对应（头文件放 `include/`，实现放 `src/`）。

## 3. 核心运行时对象（服务端）

组合关系（`GameSession` 是**组合根**，持有并注入全部组件；依赖用**引用**）：

```
GameSession  ── 拥有 ──▶ EventBus
             ── 拥有 ──▶ GameModel ──▶ SeatManager / CardsManager / PotManager / BettingRound
             ── 拥有 ──▶ GameController ──(引用)──▶ GameModel
                                        └─(引用)──▶ EventBus
```

| 类型 | 职责 | 备注 |
|---|---|---|
| `GameSession` | 组合根：组装并拥有全部组件 | 引用注入，不暴露裸指针 |
| `GameModel` | 领域模型/规则；方法**返回事件**，不碰 `EventBus` | 与 `EventBus` 解耦 |
| `GameController` | 状态机：决策（`decideAfterBettingRound()`）+ `enterStage`/`enter*`；**负责发布事件** | 纯决策与副作用分离 |
| `BettingRound` | 一个下注轮的金额 + 行动顺序 + `applyAction(Event::PlayerAction) -> optional<PlayerActed>` | 合并了旧的 `ActionFlow`+`BettingManager` |
| `Player` | **筹码与 all-in 的唯一入口**：`payChips`/`winChips`/`commit`/`refund` | 含 `assert`；维护 `ALL_IN`/`ACTIVE` |
| `PotManager` | 底池（主池/边池） | 原 `ChipsManager`；无筹码增减薄包装 |
| `SeatManager` | 座位/按钮/行动顺序 | |
| `CardsManager` | 公共牌、牌堆、发牌 | |
| `Deck` | 牌堆，**可播种**便于复现 | |
| `EventBus` | 进程内事件总线（入站为主） | 见 `DECISIONS.md` |

## 4. 事件流

**入站**（客户端命令 → 领域）：

```
Client ─▶ ClientNetwork ─▶ (反序列化) ─▶ EventBus ─▶ GameController ─▶ GameModel
```

**出站**（领域事件 → 客户端）：

```
GameController ──(发布 GameEvent)──▶ ServerIoAdapter ──▶ (序列化) ─▶ ServerNetwork ─▶ Client
```

> ⚠️ 出站目前由 `ServerIoAdapter` **订阅 `EventBus` 后转发**，被评估为"隐式、易漏"。
> 拟改为：`core` 定义**显式出站端口** `ClientGateway{broadcast/sendTo}`，由 `GameController` 在事件产生处显式调用，应用层实现（网络/控制台/测试）。
> 状态：**Proposed**，见 `docs/DECISIONS.md`（待定）。

**事件类型**（`Event.hpp`）：`PlayerJoinRequest` `NewPlayerJoined` `PlayerJoinResponse` `StartGame` `GameStarted` `HoleCardsDealt` `StageChanged` `BlindPosted` `CommunityCardsDealt` `BettingRoundStarted` `BettingRoundEnded` `WaitingForAction` `PlayerAction` `PlayerActed` `HoleCardsRevealed` `PotSplited` `WinnerDeclared` `UncalledBetReturned` `GameEnded`；网络侧 `ClientConnected` `ClientDisconnected` `ServerDisconnected`。

## 5. 关键不变量

- **all-in 是玩家状态，不是动作意图**；由 `Player` 在 `payChips`(扣到 0)/`winChips`/`refund` 维护 `ALL_IN`/`ACTIVE`。
- **未跟注筹码在每次下注轮结束**时退还。
- **无人竞争（alive==1）**时，整池直接判给唯一存活者，**不切分底池**。
- **摊牌顺序**：任一人 all-in → 一次全亮（TDA 16）；否则从河牌轮最后激进者（无则按钮左侧）起顺时针逐个亮（TDA 17）；暂不实现 muck。
- **结算后清零** `committed`。
- **筹码守恒**：任一时刻，玩家筹码 + 各池筹码 = 初始总量（回归测试见 `test/invariants_test.cpp`）。

## 6. 构建目标状态

| 目标 | 状态 | 说明 |
|---|---|---|
| `poker_core` | ✅ 可编 | 核心静态库 |
| `poker`（demo） | ✅ 可编 | 控制台 demo，接受随机种子 |
| `poker_test` | ✅ 可编 | GoogleTest 用例 |
| 应用层目标（`application`/`presentation` 等） | ⚠️ **当前编译不过** | 序列化字段漂移、旧 `Action` 命名等，**暂缓**，见 `docs/ROADMAP.md` |
| `src/server_main.cpp` / `src/client_main.cpp` / `ui_test/` | ⚠️ 旧代码 | 待应用层整理时处理 |

## 7. 复现与调试

- `Deck` 支持播种；demo 可打印并接受种子：`./cmake-build-debug/poker 12345`。
- 随机相关缺陷请**固定种子**复现，并把种子写进报告。
