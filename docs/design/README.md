# docs/design · 设计提案与讨论笔记

> 这里放**设计过程**：背景 · 备选方案 · 取舍 · 结论。
> 与其它文档的分工：

| 文档 | 记什么 | 何时写 |
|---|---|---|
| `docs/design/*.md` | 一个主题的**设计过程**与结论 | 动手前 / 设计期间 |
| `docs/architecture.md` | 当前架构的**结果**：模块 / 依赖 / 数据流 | 结构变化后 |
| `docs/roadmap.md` | **待办**与优先级 | 持续 |

> 注：本项目不再单独维护 `DECISIONS.md`；**结论写在对应提案末尾**，结构变化同步进 `docs/architecture.md`。

## 命名与生命周期

- **主题提案用主题名**（如 `reopen-betting.md`、`event-protocol.md`），自解释、不编号。
- **日期命名的文件**（`2026-10-21.md`、`2026-10-22.md`）是当时的**讨论笔记**，保留原貌，不强制改成主题名。
- 生命周期：`讨论/提案` → 结论落地为代码 + 更新 `docs/architecture.md` → 提案保留作历史。
- 被推翻的提案：在开头标注 **Superseded**（及替代方案），原文不改。

## 索引

| 文件 | 主题 | 状态 | 关联 |
|---|---|---|---|
| [`reopen-betting.md`](reopen-betting.md) | 重开下注（TDA 47）：规则、易错例、算法 | 结论已定（实现待做） | roadmap P0 |
| [`event-protocol.md`](event-protocol.md) | 事件协议修订：按方向拆分 + 去派生字段 | **部分落实** | architecture §5 |
| [`client-gateway.md`](client-gateway.md) | 显式出站端口 `ClientGateway` | **Superseded**（改为 4 端口：`ClientGateway`/`ServerGateway` + `ClientHandler`/`ServerHandler`） | `2026-10-22.md` |
| [`2026-10-22.md`](2026-10-22.md) | 讨论笔记：IServer / IClient + ClientHub | **已落地（后演进为 4 端口 + on）** | architecture §2 |
| [`2026-10-22-review.md`](2026-10-22-review.md) | 评审：IClient / IServer 设计（P0 差距与建议） | 评审记录 | roadmap P3 |
| [`2026-10-21.md`](2026-10-21.md) | 讨论笔记：Player 字段分散、拆分 GameModel | 未来方向 | `state-composition.md` |
| [`state-composition.md`](state-composition.md) | 状态组合化（拆分类组件 / 函数式核心） | **未来方向（暂缓）** | roadmap P5 |
| `player-intent-model.md` | 显式玩家行动模型 / `betting.h` | 待补（代码已有草稿） | roadmap P5 |
