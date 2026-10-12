# docs/design · 设计提案

> 这里放**进行中/已定稿的设计提案**（设计过程：背景 · 方案 · 取舍 · 结论）。
> 与其它文档的分工：

| 文档 | 记什么 | 何时写 |
|---|---|---|
| `docs/design/*.md`（本文档所在） | 一个主题的**设计过程**：备选方案、取舍、论证 | 动手前 / 设计期间 |
| `docs/DECISIONS.md` | **定下来的决策**（ADR）：定了什么 + 为什么 | 结论产生时（引用对应设计提案） |
| `docs/ARCHITECTURE.md` | 当前架构的**结果**：模块/依赖/数据流 | 结构变化后 |
| `docs/ROADMAP.md` | **待办**与优先级 | 持续 |

## 命名与生命周期

- **文件名用主题**（如 `reopen-betting.md`），**不编号**——避免与 `DECISIONS.md` 的 ADR 编号（0001…）冲突，主题名自解释。
- 生命周期：`提案（本目录）` → 定稿后**在 `DECISIONS.md` 增加一条 ADR**（链接本提案）→ 若改变结构，**同步更新 `ARCHITECTURE.md`**。
- 提案被推翻时：保留原文，在开头标注 `Superseded by ADR-NNNN`。

## 索引

| 提案 | 主题 | 状态 | 关联 |
|---|---|---|---|
| [`reopen-betting.md`](reopen-betting.md) | 重开下注（TDA 47）：规则、易错例、算法 | 结论已定（实现待做） | ROADMAP P0 |
| [`client-gateway.md`](client-gateway.md) | 显式出站端口 `ClientGateway` | Proposed | DECISIONS 0011 |
| [`event-protocol.md`](event-protocol.md) | 事件协议修订：按方向拆分 + 去派生字段 | Proposed | DECISIONS 0011 / 0012 |
| [`state-composition.md`](state-composition.md) | 状态组合化（拆分类组件 / 函数式核心） | **未来方向（暂缓）** | ROADMAP P5 |
| （待补）`player-intent-model.md` | 显式玩家行动模型 / `betting.h` | Proposed | DECISIONS 0012 |

> 注：`player-intent-model` 目前只在 `DECISIONS.md` 与对话中，后续按需整理成正式提案。
