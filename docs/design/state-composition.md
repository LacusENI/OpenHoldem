# 设计提案：状态组合化（拆分类组件，而非单一 `GameState`）

> 状态：**未来方向（Deferred）** · 2026-10-12
> ⚠️ **现在不做**：实现时机过早。等**真正开始搭建客户端 / 需要两端共享状态与规则**时再启动。
> 关联：`docs/note/2026-10-22.md`（`IClient` / `IServer`）、`docs/glossary.md`、`docs/roadmap.md`。
> 说明：本提案只取 ECS 的**"组合"**思想，**不是 ECS**——没有实体 id、没有"系统遍历组件集合"。

## 1. 背景与动机

- 当前状态散落在若干 manager（`BettingRound` / `PotManager` / `SeatManager` / `CardsManager`）里，且它们以**引用互相持有**（状态与行为耦合）。
- 客户端 `ClientModel` 又**重复**了一份公开状态（`players` / `player_bets` / `pot` / `community_cards`），导致字段与规则**两处维护**。
- 目标：把"状态"抽成**按内聚拆分的值组件**，由一个聚合对象组装；**纯函数只吃它真正需要的组件** → 便于两端复用、替换与单测。

## 2. 原则：按三条轴拆

| 轴 | 怎么拆 | 收益 |
|---|---|---|
| **① 生命周期 / 作用域** | session 级 / 手牌级 / 下注轮级 | **reset 边界清晰** |
| **② 公开 vs 私有** | 客户端可见 / 仅服务端 | **这就是两端复用的机制** |
| **③ 内聚 + 原子不变量** | "总是一起变、必须一致"的量归一处 | 避免跨组件穿透 |

> **硬纪律**：必须同时保持一致的量**绝不拆开**。例如 `current_bet` 与各玩家的 `round_bet` 耦合，`full_level` / `min_raise` / `acted` 亦然——它们要么都在 `Betting`，要么就会产生"改一半"的 bug。

## 3. 组件草案

| 组件 | 内容 | 生命周期 | 可见性 | reset 时机 | 现状归属 |
|---|---|---|---|---|---|
| `Roster` | 座位 / 按钮 / 盲注配置 / 玩家名单 | session | 公开 | 会话内不变 | `SeatManager` |
| `Players` | 每人 `stack` / `status` / `committed` / `round_bet` | 手牌 | 公开 | 新手牌 | `PlayerSet` / `Player` |
| `Board` | 公共牌 + `stage` | 手牌 | 公开 | 新手牌 | `CardsManager` / `GameModel` |
| `Betting` | `current_bet` / `min_raise` / `full_level` / `actor` / `acted` | 下注轮 | 公开 | 每轮 / 新手牌 | `BettingRound` |
| `Pots` | 主池 / 边池 | 手牌 | 公开 | 新手牌 | `PotManager` |
| `Deck` | 牌堆 + 各座位底牌 | 手牌 | **★私有** | 新手牌 | `CardsManager` |

**读法**：第二列给 clear 的 reset 边界；`Deck` 是**唯一私有件** → 客户端**根本不装它**。

## 4. 组装方式

**存储用聚合**（传递方便）：

```cpp
struct GameState {
    Roster  roster;
    Players players;
    Board   board;
    Betting betting;
    Pots    pots;
    // Deck deck;   // ★ 仅服务端装配
};
```

**行为用"窄参数"纯函数**（依赖写进签名）：

```cpp
std::vector<Action> legalActions(const Betting&, const Players&, seat_t);
chips_t             potSize(const Pots&);
seat_t              currentPlayer(const Betting&, const Players&);
chips_t             amountToCall(const Betting&, const Players&, seat_t);
```

好处：依赖图一眼可见；单测**只构造用到的组件**，不用造整个 `GameState`。

**数据布局**：玩家数只有 2~9 → 用 **AoS**（`std::vector<Player>`，可读优先）。ECS 的 SoA 只在海量实体时才有意义。

## 5. 两端复用（本提案的最终收益）

```
client 的 state = 公开组件 + 「自己的底牌」        // 无 Deck、无权威
server 的 state = 公开组件 + Deck(牌堆/全部底牌)   // 权威 + 秘密
```

- 同一批公开组件、同一套纯函数，被两端**各自组装**（呼应"函数式核心 + 命令式外壳"）。
- 客户端成为 **reducer**：`state = next(state, event)`（见同步方案讨论）。

## 6. 与现有代码的增量迁移路径（不重写）

1. **抽 `GameState`（公开部分）**，现有 manager 保持不变（先"包一层"）。
2. 逐步把 manager 的**状态**搬进组件、**行为**搬成**纯函数**。
3. 客户端改用**共享组件 + reducer**（顺带干掉 `ClientModel` 的重复字段与 `PlayerStatus` 双枚举）。
4. 最后再拆**私有件**（`Deck`）。

> 每一步都应**独立可编译、测试全绿**、可单独提交。

## 7. 拆错的信号

1. **贫血 + 穿透**：组件全是空 struct，函数却要 `(roster, players, board, betting, pots)` 全拿 → 没按内聚拆。
2. **原子不变量被拆散**：`current_bet` 与 `round_bet` 分离、需手工同步 → 合并。
3. **重复缓存**：拆完后又到处存派生副本 → 派生值**永远现算**，不落地。
4. **过度细分**：每字段一个 struct、跨组件引用爆炸 → 按"会不会一起变"合并。

## 8. 验收判据

- [ ] 每个组件有**明确归属**（谁负责改它）；
- [ ] **跨组件读**很少且显式；
- [ ] 每个组件的 **reset 时机**清楚；
- [ ] **公开/私有**一目了然（客户端能装哪些件）；
- [ ] 单测**不用**构造整个 `GameState`；
- [ ] `ClientModel` 不再重复存储公开状态。

## 9. 何时才做（触发条件）

- 开始搭建**客户端**、需要两端共享状态 / 规则 / 查询时；
- 或要加**玩法变体**（Omaha / 限注），需要更清晰的规则与状态边界时。

## 10. 非目标

- ❌ 不做 ECS 的实体 / 系统机器；
- ❌ 不引入插件框架 / 系统注册表；
- ❌ 不为"漂亮"提前重写现有 manager。
