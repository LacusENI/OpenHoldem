# 设计提案：事件协议修订（按方向拆分 + 去派生字段）

> 状态：**Proposed**（待评审）
> 关联：`docs/design/2026-10-22.md`（`IClient` / `IServer`）；`docs/architecture.md` §5。
> 准则：**协议传事实与意图，不传派生量**；权威在服务器，客户端只做投影（派生量由客户端计算）。
> `NewPlayerJoined` `PlayerJoinResponse` `GameStarted`
> `HoleCardsDealt` `StageChanged` `BlindPosted` `CommunityCardsDealt`
> `PlayerActed` `HoleCardsRevealed` `UncalledBetReturned` `GameEnded`保持不变。
> 改名：`NewPlayerJoined` -> `PlayerJoined`, `PlayerJoinResponse` -> `JoinResponse`
> 记法：`事件名 {字段名, ...}`，括号内注明原事件名。

## 1. 入站（客户端 → 服务器，意图）

- `JoinRequest {uid}` — 原 `Event::PlayerJoinRequest`，改入 `Command::`。
- `SubmitAction {target_bet, is_fold}` — 原 `Event::PlayerAction`，改入 `Command::`；意图协议见 `betting.h` / roadmap P5。
- `StartGame {}` — 补入入站 variant（原先不在 `GameEvent` 中，无法上线）。
- 命令变体：`GameCommand = std::variant<JoinRequest, StartGame, SubmitAction>`，供 `ServerHandler::on(command, sender)` 使用。
- 身份：`ClientSession { client_id_t id; ClientHandler& handler; }` 由服务器侧适配器提供；客户端 `ServerGateway::send(command)` 不带身份。

## 2. 出站（服务器 → 客户端，事实 / 裁定 / 私有）

- `TurnStarted {player}` — 原 `Event::WaitingForAction`；删除派生字段 `player_chips` `player_bet` `round_bet` `min_raise` `can_raise`（由客户端推导）。

## 3. 待定（倾向如此，未拍板）

- `PotsAwarded {pot_index, amount, award_to}` — 合并原 `PotSplited`（`pot_chips`）+ `WinnerDeclared`（`pot_index`, `winners`），每个底池一条。
- `BettingRoundEnded {stage}` — 候选删除（改由客户端从事实流派生）。
- `BettingRoundStarted {stage}` - 候选删除

> 当前状态（2026-10-22）：
> - 协议已按 `Command::` / `Event::` 方向拆分，`PlayerJoined` / `JoinResponse` 已改名，`PotAwarded` 已合并。
> - `Event::TurnStarted` 仍保留派生字段（临时）。
> - `JoinRequest` 当前用 `{uid}`；`SubmitAction` 当前为 `{target_bet, is_fold}`（尚无 `is_all_in`）。