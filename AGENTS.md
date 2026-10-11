# AGENTS.md

> 面向 AI 编码代理的项目契约。**每个会话先读本文件**；深入内容见 `docs/`。

## 项目简述

C++20 德州扑克（当前处于 `demo-v9` 重构期）。
目标：补完核心业务逻辑，最终支持局域网联机对战。
技术栈：CMake + Ninja + vcpkg ｜ clang ｜ GoogleTest ｜ SFML(网络) ｜ nlohmann/json(序列化)。
补充：个人项目，仅用于学习。

## 当前阶段与范围边界（重要）

- **重心：补完核心业务逻辑**，即 `include/core/**` + `src/core/**`。
- **默认只改**：`include/core/**`、`src/core/**`、`test/**`、`docs/**`。
- **应用层暂时不管**：`network/`、`presentation/`、`serialization/`、`logging/`、`src/*_main.cpp`、`ui_test/` 目前**可能编译不过**——不要顺手去修，除非任务明确要求。
- 项目处于**快速迭代期、重构频繁**：以清晰/正确为先；测试**酌情**添加（见协作规则）。

## 协作规则（必须遵守）

1. **不要自动 `git commit`**。完成改动后停下，等用户审核后再由用户决定提交。
2. **不要自作主张地添加或修改测试**。实现/修改功能后先交用户审核；是否补测试、补哪些由用户决定（除非用户明确要求写测试）。
3. **复杂任务先写 Coding Plan**：动手前给出（目标 · 涉及文件 · 步骤 · 验收标准），与用户讨论确认后再执行。
4. 新增**用户可见的落盘文档**用**中文**写入 `docs/`。
5. 提交信息格式见下；**由用户触发提交时**才使用。
6. 备注：目前 docs 由 agent 生成，没有完整审查，可能存在错误。

## 构建 / 测试 / 运行

```bash
# 配置（含测试）
cmake --preset debug-with-tests
# 构建
cmake --build --preset debug
# 测试（核心改动后应全绿）
ctest --preset test

# 或直接针对目标
cmake --build cmake-build-debug --target poker poker_test
ctest --test-dir cmake-build-debug

# 运行 demo（可带种子以复现随机序列）
./cmake-build-debug/poker [seed]
```

## 目录与模块速览

全部代码在 `include/` + `src/` 下，分三层：

- **core（核心，纯领域，不依赖网络/序列化）**：`common`(基础类型) · `event`(事件与总线) · `model`(领域模型) · `server`(服务端领域) · `client`(客户端) · `algo`(算法) · `interface`(端口)
- **application（应用层）**：`network` · `serialization` · `logging` · `presentation` + `src/*_main.cpp`
- **test**：GoogleTest 用例

详见 `docs/ARCHITECTURE.md`。

## 编码约定

- **显式优于隐式**：合法则接受，非法则**拒绝/断言**，**不要静默改写**。
- **早失败**：关键路径加 `assert`，别让错误悄悄传播。
- 命名空间 `poker`（实验性下注模型草稿用 `poker::betting`）。
- 所有权用值/引用表达；非拥有引用注入（组合根 `GameSession` 持有全部组件）。
- 头文件 `#pragma once`；公共头文件顶部可写**简短文件级 `@brief`**（职责 + 契约）。**不要求**每个文件都写"总纲"，更不要写"是什么"的流水账。
- 风格与周围代码保持一致。

## 提交信息格式

```
type(scope): 中文描述

- 详情条目
- 详情条目
```

`type` 用 `feat/fix/refactor/chore/docs/test/...`；`scope` 常用 `domain`。

## 先读哪些文档

- `docs/ARCHITECTURE.md` — 架构 / 模块 / 数据流 / 不变量
- `docs/DECISIONS.md` — 已定决策（ADR）。**改设计前先查这里有没有冲突**。
- `docs/ROADMAP.md` — 路线图与当前待办
- `docs/GLOSSARY.md` — 术语表（通用语言，人和 AI 对齐用）
- `docs/TESTING.md` — 测试策略
- `docs/design/` — 设计提案（过程）
- `README.md` — 人类视角：是什么 / 怎么跑

## 关键不变量 / 易错点（务必遵守）

- **all-in 是"玩家状态"，不是动作意图**：由 `Player` 在 `payChips`(扣到 0) / `winChips` / `refund` 维护 `ALL_IN` / `ACTIVE`。
- `Action` 枚举用 CamelCase：`Void / Fold / Check / Call / Bet / Raise / Undercall / ShortAllIn`（已删除 `ALL_IN_CALL` / `ALL_IN_RAISE`）。
- **未跟注筹码在每次下注轮结束时退还**。
- **无人竞争（alive==1）时整池直接判给唯一存活者**，不切分底池。
- **摊牌顺序**：任一人 all-in → 一次全亮（TDA 16）；否则从河牌轮最后激进者（无则按钮左侧）起顺时针（TDA 17）；暂不实现 muck。
- **结算后清零** `committed`。
