# 重开下注（Re-Opening the Bet）规则说明

> 状态：**结论已定，实现待做**（见 `docs/roadmap.md` P0）。
> 目的：把 TDA "Re-Opening the Bet" 规则、常见误解、易错例子与正确实现（含伪代码与等价算法）讲清楚，供人或 AI 阅读/实现。
> 适用范围：无限注（no-limit）/ 底池限注（pot-limit）德州扑克的一轮下注。
> 相关规则：TDA Rule 47（2022/2024；早年版为 Rule 44 / 37-38）、Robert's Rules of Poker。

---

## 1. 术语（ubiquitous language）

- **一轮下注（betting round）**：从发牌到该轮下注结束。
- **当前下注额 `current_bet`**：本轮目前需要跟到的最高额度。
- **完整下注/加注（full bet / full raise）**：加注量达到本轮"最小加注量"的下注/加注。
- **最小加注量（min raise）**：本轮*最后一次完整有效加注的加注量*（不是相对最初下注的差）。
- **短全下 / 不完整加注（short all-in / incomplete raise）**：全下但额度不足以构成一次完整加注。
- **行动开放 / 关闭 / 重开（open / closed / reopen）**：
  - 一轮开始时，每位在局玩家的下注权是 **open** 的；
  - 某玩家**行动后（自愿下注/加注/跟注/过牌）**，其下注权变为 **closed**；
  - 只有当之后出现一次**完整**下注/加注时，其下注权才会被 **reopen**。

一句话：**"重开"= 该玩家重新获得"可以加注"的权利。**

---

## 2. 规则（自然语言）

### 2.1 TDA 原文（Rule 47）

> **47: Re-Opening the Bet**
> **A:** In no-limit and pot limit, an all-in wager (**or cumulative multiple short all-ins**) totaling less than a full bet or raise **will not reopen betting** for players who have already acted and are not facing at least a full bet or raise when the action returns to them. If multiple short all-ins re-open the betting, the minimum raise is always the last full valid bet or raise of the round (See also Rule 43).
> **B:** In limit, at least 50% of a full bet or raise is required to re-open betting for players who have already acted.

### 2.2 逐句拆解

1. **触发条件是"完整"**：只有**完整**下注/加注才重开；不足额的 all-in **不重开**。
2. **是"逐人"判定**：不重开只针对"**已经行动过、且行动轮回到他时面对的不是至少一个完整加注**"的玩家。若他面对的是完整加注，则重开。
3. **累计也算**：**多个 short all-in 累加**若对该玩家构成一个完整加注，**也会重开**（"cumulative multiple short all-ins"）。
4. **最小加注量不变**：多个 short all-in 重开后，最小加注量仍是"**本轮最后一次完整有效下注/加注的加注量**"。
5. （限注）用 50% 阈值；无限注用 **100%**。

### 2.3 一句可达的直觉

维护两个量：**`full_level`（上一次完整加注后的高度）** 与 **`min_raise`（那次完整加注的加注量）**。
**重开阈值 = `full_level + min_raise`。**
- 单次加注量 `>= min_raise` → 完整加注 → 重开；
- 或者 `current_bet`（本次达到的高度）**越过** `full_level + min_raise` → 累计 short all-in 凑满一个完整加注 → 重开；
- 否则不重开。

---

## 3. 为什么容易错

- **误以为是"任意 all-in 都重开"**：错。不足额的 all-in 不重开。
- **误以为是"整轮一次性判断"**：错。是**逐人**判断（有人被重开，有人没有）。
- **漏掉"累计多个 short all-in"**：两个都不足额的全下，累加起来够一个完整加注时，**同样重开**。
- **把"最小加注量"算成"相对最初下注的差"**：错。它永远是**最后一次完整有效加注的加注量**。
- **用一个布尔标记"最近一次是不是 short all-in"**：错。布尔无法表达"累计"，必须保存**上一次完整加注的水平**。

---

## 4. 易错例子（重点）

**盲注 100/200，翻前。** 依次发生：

| 步骤 | 动作 | 关键计算 |
|---|---|---|
| 1 | **A 跟注 200** | A 行动过（A 的下注权 closed） |
| 2 | **B 加注到 500** | 加注量 300 ≥ 最小加注 200 → **完整加注** → **重开**（所有人重新获得加注权）；随后 B 自己行动过 |
| 3 | **C 跟注 500** | C 行动过（closed） |
| 4 | **D 全下到 600** | 从 500 加注量 100 < 最小加注 300；且 600 < (500+300)=800 → **纯 short all-in，不重开** |
| 5 | 行动回到 A | A 自 B 的完整加注之后**没有行动过** → **A 可以加注** ✅ |

**结论**：
- **A 可以加注**（B 的完整加注重开了 A 的权利）；
- **B、C 不能加注**（他们行动之后只面对 D 的 short all-in，未重开）。

> 这正是"逐人"的体现：同一时刻，A 能加注而 B、C 不能。

### 4.1 另一个"累计"例子

翻后，最小下注 200：A 下注 200（完整，`full_level=200, min_raise=200`）→ B 跟注 200 →
- C 全下 300：`delta=100 < 200`，且 `300 < 200+200=400` → 不重开；
- D 全下 400：`delta=100 < 200`，但 `400 >= 200+200=400` → **累计凑满 → 重开**；
- 此时 **B 可以加注**（D 的累计动作重开），而 C 不能。

---

## 5. 状态与伪代码

### 5.1 每轮状态

```
State (per betting round):
    current_bet   # 当前最高下注额
    full_level    # 上一次完整有效加注后达到的高度
    min_raise     # 上一次完整有效加注的加注量（本轮最小加注量）
    acted[p]      # 玩家 p 自上次"重开"以来是否已（自愿）行动
    in_hand[p]    # p 是否仍在局（未弃牌、未出局）
    stack[p]      # p 剩余筹码
```

初始化：

```
init_preflop(bb):            # bb = 大盲
    current_bet = full_level = min_raise = bb
    acted[*] = false

init_postflop(min_bet):     # 通常 min_bet = bb
    current_bet = 0
    full_level  = 0
    min_raise   = min_bet
    acted[*]    = false
```

> 注：盲注是被迫投入，**不算**"自愿行动"。

### 5.2 处理一次"抬高 current_bet 的动作"

```
# 前提：调用方已保证——要么这是合法的"完整加注"(加注量 >= min_raise)，
#       要么这是一次全下。非全下的"不足额加注"是非法的，应在此之前拒绝。
on_wager(p, to_amount):
    delta = to_amount - current_bet

    if delta >= min_raise:
        # 单次完整加注
        full_level = to_amount
        min_raise  = delta
        reopen()
    elif to_amount >= full_level + min_raise:
        # 累计 short all-in 凑满一个完整加注（Rule 47 第一句）
        full_level = to_amount
        # min_raise 不变：永远是本轮最后一次完整有效加注的加注量（Rule 47 末句 + Rule 43）
        reopen()
    # else: 纯 short all-in —— 只推进 current_bet，不重开

    current_bet = to_amount

reopen():
    for q in in_hand: acted[q] = false      # 所有人重新获得加注权

# 玩家 p 做不改变 current_bet 的行动（跟注/过牌）
on_act(p):
    acted[p] = true

canRaise(p):
    return in_hand[p]
           and stack[p] > 0
           and (current_bet == 0 or not acted[p])
```

> `canRaise` 的三段：在局、还有筹码、且"自上次重开以来没行动过"（或当前无人下注，允许下注）。

---

## 6. 三种等价算法

三者都在回答同一个问题：**"自玩家 p 上次行动以来，是否发生过一次'重开'？"**

### 算法 1：玩家标志位（推荐，最直观）

- 每个玩家维护 `acted_since_reopen[p]`；
- **重开事件**发生时，把所有在局玩家的该标志置 `false`；
- 玩家行动时置 `true`；
- `canRaise(p) = !acted_since_reopen[p] && in_hand[p] && stack[p] > 0 && (current_bet == 0 || ...)`。

### 算法 2：基线快照

- 玩家 p 行动时，记录当时的 `(baseline_full, baseline_min) = (full_level, min_raise)`；
- `canRaise(p) = current_bet >= baseline_full + baseline_min`（外加在局/有筹码）。
- 直觉：p 行动时的重开阈值是 `full_level+min_raise`；只要之后 `current_bet` 越过它，就说明发生过重开。

### 算法 3：重开计数器

- 全局 `reopen_count`，每次重开事件 `+1`；
- 玩家 p 行动时记录 `seen_reopen[p] = reopen_count`；
- `canRaise(p) = reopen_count > seen_reopen[p]`（外加在局/有筹码）。

### 等价性说明

"重开事件"的定义（单次完整加注，或 `current_bet` 越过 `full_level + min_raise`）对三者一致：
- 算法 1 直接对"重开"做计数/标志；
- 算法 3 把"标志"换成"计数器增量"（等价改写）；
- 算法 2 把"阈值"在玩家行动时快照下来，之后只需比较 `current_bet`（因为阈值在重开前不变）。

---

## 7. 边界与陷阱

1. **初始化**：翻前 `min_raise = BB`、`full_level = BB`；翻后 `full_level = 0`、`min_raise = 最小下注(通常 BB)`。
2. **`on_wager` 的前提**：非全下的"不足额加注"必须**先被拒绝**；否则会被误判为 short all-in。
3. **`acted` 只统计"自愿行动"**：盲注不算。
4. **逐人属性**：同一时刻，有人能加注、有人不能（见 §4 例子）。
5. **累计**：多个 short all-in 可累加重开；`min_raise` 在累计重开后**保持不变**。
6. **all-in 玩家**：他们无筹码、不能再行动，但属于"在局"（用于边池判定），`reopen` 时重置其 `acted` 无害。
7. **面对"完整加注"但尚未行动者**：本来就可以加注（`acted` 为 false）。

---

## 8. 与本项目现状的关系

当前 core 的实现（`BettingRound`）：

- 用一个布尔 `has_incomplete_raise`：
  - 每次 `Action::ShortAllIn` 置 `true`（**不累积**）；
  - 只有完整加注（`Raise/Bet`）置 `false`；
- `canPlayerRaise(seat) = !(player.isActed() && has_incomplete_raise)`。

**缺陷**：布尔无法表达"累计"，且没有"逐玩家基线"，因此**没有实现"多个 short all-in 累计重开"**。
性质：**偏严**（只会拒绝本应允许的加注，不会错误放行），不是崩溃类问题。

**修法方向**：引入 `full_level`（上一次完整加注的水平），把"能否加注"改成**逐玩家**判定（§6 算法 1），并在 `on_wager` 里加入 §5.2 的累计分支与回归测试。

---

## 9. 实现检查清单

- [ ] 每轮状态含 `current_bet / full_level / min_raise / acted[*]`，并按 §5.1 初始化。
- [ ] `on_wager` 实现"单次完整加注"与"累计凑满"两个重开分支，其余为纯 short all-in。
- [ ] 非全下的不足额加注在进入 `on_wager` 前被拒绝。
- [ ] `reopen()` 重置所有在局玩家的 `acted`。
- [ ] `canRaise(p)` 为逐人判定。
- [ ] 回归测试覆盖：
  - [ ] 单次 short all-in 不重开；
  - [ ] 累计两个 short all-in 凑满 → 重开；
  - [ ] §4 的"A 能、B/C 不能"例子；
  - [ ] 完整加注重开；
  - [ ] 多个 short all-in 重开后 `min_raise` 保持不变。

---

## 附：参考

- Poker TDA, **Rule 47 "Re-Opening the Bet"**（2022/2024；旧版 Rule 44 / Rule 37-38）。
- Robert's Rules of Poker：*"An 'all-in' bet of less than a full raise does not reopen the betting to a player who has already acted."*
- 相关：TDA Rule 43（Raises / Multiple Short All-Ins）、Rule 41（Raises）。
