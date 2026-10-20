# GLOSSARY · 术语表

> 项目的"通用语言"（ubiquitous language）。**人和 AI 都以此表为准**，避免各说各话。
> 约定：每条给**一句中文定义**；内部类型/字段用 `代码体` 标注，必要时指向文件。
> 新增术语请加在这里；只在一处权威（`DECISIONS.md` 记录"为什么"）。

## A. 桌与座次

- `button` (缩写 `btn`) 按钮位，庄家位
- `big blind` (缩写 `bb`) 大盲注
- `small blind` (缩写 `sb`) 小盲注
- `UTG`(Under the Gun) 枪口位 (Preflop 第一个行动的位置)

> - `big blind`/`small blind` 在项目有三个相近语义：
>   1. 指大盲注/小盲注的金额
>   2. 指大盲注/小盲注的玩家或座次
>   3. 指玩家投入大盲注/小盲注的动作

## B. 阶段与手牌

- `street` 街 (preflop, flop, turn, river)
- `stage` 阶段 (preflop, flop, turn, river, showdown, settlement)
- `postflop` flop, turn, river 的统称
- `betting round` 下注轮
- `hand` 一手牌 (从发底牌到结算的完整一轮)
- `hand` 玩家的手牌
- `hand value` 从玩家手牌中计算出的可比较的数值，用于裁定胜负
- `game` 游戏, 含有多个 hand
- `community cards` (或 `board`)公共牌
- `hole cards` 玩家的底牌

> `hand` 在项目中多个语义：
> 1. 指一手牌，从发底牌到结算的完整一轮
> 2. 指5张公共牌 + 2张玩家的底牌 
> 3. 指从上述7张牌中选出的最高的5张牌
>
> 在 2 和 3 同时出现需避免歧义的地方，
> 一般分别用 `seven cards` 和 `five cards` 表示

## C. 行动与下注

- `action` / `action kind` 玩家行动的类型
- `intent` 玩家的行动意图
- `fold`, `check`, `call`, `bet`, `raise` 弃牌, 过牌, 跟注, 下注, 加注
- `all-in` 全押
- `undercall` 全押但不足跟注额
- `short all-in` 全押但未达到最小加注
- `round bet` 当前下注轮的最高下注
- `min raise` 最小加注的下注额度 (绝对值)
- `min raise delta` 最小加注额度 (相对值)
- `min raise to` (同 `min raise`，推荐)
- `min raise by` (同 `min raise delta`, 推荐)
- `raise to` 玩家本次行动预计加注要达到的下注额度
- `raise by` 玩家本次行动预计要加注的额度
- `player bet` 玩家下注额（玩家在本轮中已下注了多少筹码）
- `player chips` 玩家筹码数量 (玩家当前手中还有多少筹码)
- `reopen betting`重开下注, 让已行动过的玩家重新获得加注权 -- 详见 `docs/design/reopen-betting.md`
- `uncalled bet` 玩家的下注额有其他人未跟到的部分
- `committed` 贡献量，玩家在每次 hand 中积累投入的筹码数量
- `acted` 玩家是否自愿行动过（不包括盲注）
- `actve` 玩家未弃牌也未 all-in
- `alive` 玩家未弃牌
- `out` 玩家出局 (筹码耗光)
- `playing` 玩家正在参与目前进行中的 hand (包括 folded)

> - `min raise` 和 `min raise delta` 的说法将逐步弃用，
> `min raise` 改为指"最小加注"的机制
> - 区分 `min raise to` 和 `min raise by`: 
> `min raise to` = `round bet` + `min raise by`

|      玩家行为      | `action`的分类 |   `intent`的分类   |
| :----------------- | :------------: | :----------------: |
| `all-in` to `call` |     `call`     |           `all-in` |
| `all-in` to `bet`  |     `bet`      |           `all-in` |
| `all-in` to `raise`|     `raise`    |           `all-in` |
| `all-in` but short of `call` | `undercall`    | `all-in` |
| `all-in` but short of `raise`| `short all-in` | `all-in` |
| `all-in` but short of `bet`  | `short all-in` | `all-in` | 

## D. 底池与结算

- `pot` | `the pot` 底池
- `main pot` / `side pot` 主池/边池
- `showdown` 摊牌
- `muck` 指定摊牌时不亮牌弃掉 (当前未实现)
- `uncontested` / `uncontested pot` 只有一名玩家未弃牌，其将赢得所有筹码
- `split pot` 拆分底池为主池和多个边池 (如果有)
- `winner` 在某个底池的所有竞争者中手牌最大将赢得底池筹码的玩家

> `the pot` 其强调是整个由主池和边池组成的总池
> 而 `pot` 可以指 `the pot`，而指某一个主池/边池

## E. 项目内部类型（`include/core/**`）

| 名称 | 定义 |
|---|---|
| `GameSession` | **组合根**：持有并装配全部服务端组件；依赖用引用注入。 |
| `GameModel` | 领域模型/规则：方法**返回事件**，不直接发布。 |
| `GameController` | 状态机：决策 + `enterStage`/`enter*`；**负责发布事件**。 |
| `BettingRound` | 一轮下注：金额 + 行动顺序 + `applyAction(...) -> optional<PlayerActed>`。 |
| `Player` | **筹码与 all-in 的唯一入口**：`payChips`/`winChips`/`commit`/`refund`（含 `assert`）。 |
| `PlayerSet` | 玩家集合（座位 → 玩家）。 |
| `PotManager` | 底池（原 `ChipsManager`）。 |
| `SeatManager` | 座位/按钮/行动顺序。 |
| `CardsManager` | 牌堆、公共牌、发牌。 |
| `Deck` | 牌堆，**可播种**（`Deck(seed)`）以便复现。 |
| `EventBus` | 进程内事件总线（主要承担**入站**）。 |
| `GameEvent` | 事件变体（`Event::*` 与 `NetworkEvent::*`）。见 `Event.hpp`。 |
| `PlayerActed` | 玩家一次行动的结果事件（含是否全下等）。 |
| `chips_t` / `seat_t` | 筹码量 / 座位号类型。见 `GameTypes.hpp`。 |

## F. 缩写

| 缩写 | 全称 / 含义 |
|---|---|
| TDA | Poker Tournament Directors Association（规则来源）。 |
| NL / PL / FL | No-Limit / Pot-Limit / Fixed-Limit。 |
| ADR | Architecture Decision Record（见 `DECISIONS.md`）。 |
| DDD | Domain-Driven Design（本项目的设计取向之一）。 |
| SB / BB / BTN / UTG / CO / HJ / MP | 位置缩写（见 §A）。 |
