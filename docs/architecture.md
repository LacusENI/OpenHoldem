# ARCHITECTURE

> OpenHoldem 架构地图

## 1. 总览与分层

```
core/
    algo/  # 用于游戏规则的纯函数集合
    client/  # 客户端领域
    common/  # 基础值类型
    event/  # 游戏事件定义与事件系统
    interface/  # 由需要应用层实现的接口
    model/  # 游戏领域模型
    server/  # 服务器
application/
    logging/  # 日志 (依赖 spdlog)
    network/  # 网络 (依赖 SFML/Network)
    presentation/  # 表现层 
    serialization/  # 序列化 (依赖 nlohmann/json)
```

## 2. 核心运行时对象（服务端）

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

## 3. 事件流（服务器）

**入站**（客户端命令 → 领域）：

```
Client ─▶ ClientNetwork ─▶ (反序列化) ─▶ EventBus ─▶ ServerController ─▶ GameModel
```

**出站**（领域事件 → 客户端）：

```
ServerController ──(发布 GameEvent)──▶ ClientGateway ──▶ (序列化) ─▶ ServerNetwork ─▶ Client
```

## 4. 构建目标状态

| 目标 | 状态 | 说明 |
|---|---|---|
| `poker_core` | ✅ 可编 | 核心静态库 |
| `poker`（demo） | ✅ 可编 | 控制台 demo，接受随机种子 |
| `poker_test` | ✅ 可编 | GoogleTest 用例 |
| 应用层目标（`application`/`presentation` 等） | ⚠️ **当前编译不过** | 序列化字段漂移、旧 `Action` 命名等，**暂缓**，见 `docs/ROADMAP.md` |
| `src/server_main.cpp` / `src/client_main.cpp` / `ui_test/` | ⚠️ 旧代码 | 待应用层整理时处理 |
