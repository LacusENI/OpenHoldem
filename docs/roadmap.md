# ROADMAP

> 路线图与当前待办（唯一真源）。每次推进后更新。

## 当前聚焦

**`demo-v9` 重构期：补完核心业务逻辑**（`include/core/**` + `src/core/**`）。
应用层（网络/序列化/表现）**暂缓**，其编译失败属预期。

## 待办（按优先级）

### P0 · 核心正确性

- [ ] **累计 short all-in 是否重开下注**：当前 `BettingRound` 用单个 `has_incomplete_raise` 布尔，无法处理"多个短全下累计达到完整加注额"。
  - 方案：引入 `full_level` + 逐玩家 `acted` 判定。
  - 详见 `docs/design/reopen-betting.md`（规则 · 易错例子 · 伪代码 · 三种等价算法）。
  - 需补回归测试。
- [ ] **手牌 / 会话生命周期语义**：区分"一手（hand）"与"一局/赛事（session）"；补 `HandStarted` / `HandEnded`。

### P1 · 结构收敛（低风险整理）

- [ ] `ClientHub` / `ClientController` **去裸指针**（`ClientHub` 现保存非拥有 `ClientHandler*`）。
- [ ] 统一"双状态枚举"（`Player::State` 与 `PlayerStatus`）。
- [ ] `seat` 类型 / `optional` 化收敛。
- [ ] `const` 与命名统一。

### P2 · 测试加固

- [ ] **恢复/重写不变量测试**（`invariants_test.cpp` 已被删除）：随机多手，断言筹码守恒、状态合法（`Deck` 可播种）。
- [ ] 把 `ServerController::decideAfterBettingRound()` 提为**可单测的纯函数**并补用例（当前为私有成员）。

### P3 · 应用层（"打通局域网联机"）

**联机基座（上网络前必须；评审见 `docs/note/2026-10-22-review.md`）**

- [x] **4 端口 + 身份**：`ClientGateway` / `ServerGateway`（出站）+ `ClientHandler` / `ServerHandler`（入站）；`ServerHandler::on(GameCommand, ClientSession)` 的 sender 由服务器侧适配器提供；`client_id_t` 已加入。
- [ ] **校验发送者座位**：维护 `ClientId → seat` 映射；在 `handle(SubmitAction)` 中校验"提交者 == 当前行动者"（不再依赖 `ClientHub::seatOf`）。
- [ ] **补错误与生命周期**：`JoinRejected`、`ActionRejected{reason}`、断线/超时（`ClientHub::sendTo` 不再 `assert`）、`Command::NextHand`。
- [ ] **序号 / `hand_id`**：为断线重连、丢包检测、幂等铺路。
- [ ] **序列化 dispatch 统一**：`EventSerializer` 仍是逐事件 if 链（且已过期），随序列化对齐一并处理。
- [ ] **网络适配器命名**：`RemoteServer` / `RemoteClient` → `ServerProxy` / `ClientProxy`。

**接入与收尾**

- [x] **两端接口化**：4 端口（`ClientGateway` / `ServerGateway` + `ClientHandler` / `ServerHandler`）+ `ClientHub` / `LocalLink`（本地异步回环）。
- [ ] **序列化对齐**：`EventSerializer` 与最新 `Event` / `Command` 字段一致。
- [ ] **结构化事件日志**：统一为"序列化 `GameEvent` 再打印"，消除逐事件格式串。
- [ ] **重写 `src/server_main.cpp` / `src/client_main.cpp`**（当前仍 include 已删除的 `*IoAdapter.h`）。
- [ ] 修复 `presentation`（`ConsoleView` / `Formatter`，旧 `Action` 命名）。
- [ ] CMake 选项化（核心 / 应用 / 测试目标分离）+ CI。

### P4 · 功能

- [ ] **简单 AI 对手**（随机/半随机策略），让单机可玩（demo 已有 check/call bot 雏形）。
- [ ] **控制台 UI 增强**（彩色输出 / 桌面排版）。
- [ ] **锦标赛模式**（盲注递增、淘汰、多桌）。
- [ ] **图形界面**（Qt 或 SFML）。

### P5 · 待定实验 / 未来方向（暂缓）

- [ ] `betting.h` 意图模型**接入引擎**：现已实现 `process(Fold/Check/Call/Bet/Raise/AllIn)`，但引擎仍走 `SubmitAction{target_bet, is_fold}`。
- [ ] **状态组合化**：见 `docs/design/state-composition.md`。**暂缓**，等开始搭客户端 / 需要两端复用时启动。

## 已完成里程碑

- 阶段 0：低风险快修（`GlobalLogger` no-op、include 补齐、`Card`/`next_stage` 防御、`evalPatternHandType` constexpr、死代码清理）。
- 无人竞争整池结算（`a0a4eb5`）。
- 未跟注退还 + 拆分 `GameController`（`5ce14c7`）；摊牌顺序（`9799684`）。
- `BettingRound` 合并（`53b0405`）；`applyAction -> optional<PlayerActed>`（`3c95bfa`）；`Action` CamelCase + all-in 归 `Player`（`53de4b3`）。
- `ServerSession` 组合根 + 引用注入（`2847ae9`，当时名 `GameSession`）；结算清零 `committed`（`6f35547`）；`Deck` 可播种（`dd5f5d1`）。
- 状态机化（`3aab9fd`）；`BettingRoundStarted` 携带 `stage`（`3ee48f1`）。
- `PotManager` 改名 + 删薄包装（`7fe7c82`）；`GameModel` 与 `EventBus` 解耦（`4b9648d`）；关键路径加 `assert`（`c7458c9`）。
- 文档体系初稿（`b4599fe`）；事件协议 / 函数式核心 / `betting.h` 草稿（`344cc3a`、`04ed04f`、`757a645`）。
- `ClientGateway` + `Logger`（`9024092`）；`GameController` → `ServerController`、`GameSession` → `ServerSession`（`bcb04a5`）。
- `ClientGateway` 接入 `ServerController`（`1985ffa`）。
- **`IClient` / `IServer` / `ClientHub` / `LocalLink` + 本地异步回环**（`ada8cd1`；当时命名，后拆为 4 端口）。
- **接口拆为 4 端口**：`ClientGateway` / `ServerGateway`（出站）+ `ClientHandler` / `ServerHandler`（入站）；新增 `ClientSession` / `client_id_t`；`LocalLink` 改为 `serverSide()`=ClientHandler、`clientSide()`=ServerGateway（本轮，未提交）。
