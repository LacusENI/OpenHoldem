# 设计提案：显式出站端口 `ClientGateway`

> 状态：**Superseded**（未采用；实际落地为 4 端口，见 `docs/note/2026-10-22.md` 与 `docs/architecture.md`）
> 关联：拟替代 `include/core/interface/ServerIoAdapter.h`（已删除）
> 范围：服务端"**领域 → 客户端**"的**出站**通道（不含入站命令、不含网络实现细节、不含序列化格式）

## 1. 背景与问题

当前出站由 `ServerIoAdapter` 实现：它**订阅 `EventBus` 的若干事件**，在各自的 `handle` 里决定 `sendToAllClient`（广播）还是 `sendToOneClient`（单传）。问题：

1. **路由是一张隐藏的表**：`subscribeEvent()` 列事件、每个 `handle` 决定受众，**离事件产生地很远**，新加事件极易漏配。
2. **两层间接**：本地 `EventBus` + 外部注入的 `std::function`，接线隐式且可变。
3. **私密信息经过本地总线**：`HoleCardsDealt` 每人不同，却被广播到本地总线；适配器虽单传，但任何其它订阅者（如日志）都能看到**全部底牌**，靠"自觉"保密很脆弱。
4. **网络与领域混杂**：在 `ClientConnected` 时 `publish(PlayerJoinRequest)`，把网络事件当领域事件。
5. **覆盖面漂移**：`StageChanged` / `HoleCardsRevealed` / `PotSplited` / `WinnerDeclared` / `UncalledBetReturned` 等**未订阅** → 实际出站集合与事件集合在漂移。

## 2. 目标 / 非目标

- **目标**：让"发给谁"**显式**地写在**事件产生处**（`GameController`）；出站通道可替换、可测；核心不依赖网络 / 序列化。
- **非目标**：入站命令路径（仍走 `EventBus`）；网络与序列化的具体实现；客户端的 UI。

## 3. 方案

### 3.1 端口（`core` 定义，应用层实现）

```cpp
// include/core/interface/ClientGateway.h
namespace poker {
class ClientGateway {
public:
    virtual ~ClientGateway() = default;
    virtual void broadcast(const GameEvent& event) = 0;            // 广播给所有客户端
    virtual void sendTo(seat_t seat, const GameEvent& event) = 0;  // 单传给指定座位
};
}
```

> `GameEvent`（variant）在此**是恰当的**：虚函数接口需要一个公共类型；这是 `GameEvent` 的合理用途（其它签名仍用具体事件类型）。

### 3.2 调用方式（受众写在产生处）

```cpp
void GameController::enterPreflop() {
    game_model.shuffleDeck();
    for (const auto& e : game_model.dealHoleCards())
        out.sendTo(e.player_seat, e);             // 私密：单传
    out.broadcast(game_model.startBettingRound()); // 公开：广播
    out.broadcast(game_model.postSmallBlind());
    out.broadcast(game_model.postBigBlind());
    game_model.startActionRound();
    if (auto w = game_model.awaitPlayerAction())
        out.sendTo(w->player, *w);                 // 该玩家行动：单传
}
```

### 3.3 通道划分

- **入站（客户端命令 → 领域）**：仍走 `EventBus` → `GameController` → `GameModel`。
- **出站（领域 → 客户端）**：全部走 `ClientGateway`。

即：**`EventBus` 只承担入站**，避免"两个通道可能不一致"。

### 3.4 实现与组合

| 实现 | 位置 | 用途 |
|---|---|---|
| `NetworkGateway` | `application` | 序列化 + `ServerNetwork` 发送 |
| `ConsoleGateway` | demo | 控制台打印（替代 demo 订阅总线打印） |
| `RecordingGateway` | test | 记录 `(事件, 受众)` 序列供断言 |
| `TeeGateway` | 组合 | 扇出到多个 gateway（如 `网络 + 日志`） |

日志 / 表现不再"订阅出站事件"，而是作为 gateway 的实现或经 `TeeGateway` 组合。

## 4. 事件流对比

**现在**

```
GameController ──publish──▶ EventBus ──▶ ServerIoAdapter ──▶ (序列化) ──▶ Network ──▶ Client
```

**方案后**

```
入站: Client ─▶ Network ─▶ (反序列化) ─▶ EventBus ─▶ GameController ─▶ GameModel
出站: GameController ──out.broadcast/sendTo──▶ ClientGateway ─▶ (序列化) ─▶ Network ─▶ Client
```

## 5. 取舍与备选

| 方案 | 说明 | 评价 |
|---|---|---|
| **B1 显式端口（本提案）** | `GameController` 直接调 `ClientGateway` | ✅ 路由显式、可测、符合依赖倒置 |
| A 事件携带"受众"字段 | 每个事件带 audience，单一 gateway 订阅 | ❌ 事件被塞入网络关注点；仍需维护 audience |
| C 维持现状（`ServerIoAdapter`） | 订阅 + 转发 | ❌ 见 §1 的全部问题 |

## 6. 待定点

1. **`sendTo` 用座位号还是连接 id**：现为 `client_id == 加入顺序 == seat` 的**隐式假设**；建议引入显式的 `seat ↔ connection` 登记处，别让网络层去猜。
2. **依赖注入**：`GameController` 新增 `ClientGateway&`；`GameSession` / 应用层负责注入；core-only 测试注入 `RecordingGateway`/`NullGateway`。
3. **客户端对称**：客户端也需要"网络 → 反序列化 → 本地事件 → `ClientController`"与"`PlayerAction` → 服务器"的端口，可定义对称的 `ServerPort { send(const GameEvent&); }`。
4. **私密性**：切换后，`HoleCardsDealt` 等私密事件**不必再进本地总线**，降低泄漏面。

## 7. 迁移步骤

1. 新增端口 `ClientGateway`（core）。
2. `GameController` 改为显式 `out.broadcast/sendTo`；`EventBus` 仅保留入站订阅。
3. demo / 测试提供 `ConsoleGateway` / `RecordingGateway`。
4. 删除 `ServerIoAdapter`（待应用层可用时一并处理）。
5. 同步更新 `docs/ARCHITECTURE.md` 的事件流；在 `docs/DECISIONS.md` 把 0011 由 `Proposed` 转 `Accepted`。

## 8. 验收

- `core` 可编译；`poker_test` 全绿；demo 用 `ConsoleGateway` 输出正常。
- 出站事件**全覆盖**（不再遗漏 `StageChanged` / `PotSplited` / `WinnerDeclared` 等）。
- `ServerIoAdapter` 可安全移除。
