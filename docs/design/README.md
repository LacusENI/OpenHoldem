# docs/design · 设计提案

> 这里放**设计过程**：背景 · 备选方案 · 取舍 · 结论。
> 讨论笔记 / 学习笔记 / 评审 → `docs/note/`（非正式）。
> 正式结论 → `docs/architecture.md`、`docs/roadmap.md`。

## 命名与生命周期

- **主题提案用主题名**（如 `reopen-betting.md`、`event-protocol.md`），自解释、不编号。
- 日期命名的**讨论笔记**已移到 `docs/note/`；本目录只保留主题提案。
- 生命周期：`讨论/提案` → 结论落地为代码 + 更新 `docs/architecture.md` → 提案保留作历史。
- 被推翻的提案：在开头标注 **Superseded**（及替代方案），原文不改。

## 索引

| 文件 | 主题 | 状态 | 关联 |
|---|---|---|---|
| [`reopen-betting.md`](reopen-betting.md) | 重开下注（TDA 47）：规则、易错例、算法 | 结论已定（实现待做） | roadmap P0 |
| [`event-protocol.md`](event-protocol.md) | 事件协议修订：按方向拆分 + 去派生字段 | **部分落实** | architecture §5 |
| [`client-gateway.md`](client-gateway.md) | 显式出站端口 `ClientGateway` | **Superseded**（改为 4 端口） | `docs/note/2026-10-22.md` |
| [`state-composition.md`](state-composition.md) | 状态组合化（拆分类组件 / 函数式核心） | **未来方向（暂缓）** | roadmap P5 |
| `player-intent-model.md` | 显式玩家行动模型 / `betting.h` | 待补（代码已有草稿） | roadmap P5 |

> 讨论笔记与评审见 `docs/note/`。
