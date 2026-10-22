# OpenHoldem

![C++](https://img.shields.io/badge/C++-20-blue)
![CMake](https://img.shields.io/badge/CMake-3.20-blue)
![Vcpkg](https://img.shields.io/badge/vcpkg-enabled-blue)
![Platform](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![Status](https://img.shields.io/badge/status-alpha-orange)
![Version](https://img.shields.io/badge/version-demo_v9-yellow)
![License](https://img.shields.io/badge/license-MIT-green)

一个基于 C++ 的德州扑克游戏，支持局域网联机对战。

> 当前状态：核心逻辑可运行（本地回环 demo）；局域网联机适配器与应用层整理中。

## 功能特性
- [ ] 完整的德州扑克规则（重开下注待补）
- [x] 事件驱动的游戏状态机
- [x] 服务端 / 客户端接口化（4 端口 + 本地异步回环）
- [x] All-in 与边池结算
- [ ] JSON 序列化 / 局域网联机（应用层待对齐）
- [ ] AI 对手（demo 有 check/call bot 雏形）
- [ ] 锦标赛模式
- [ ] 图形界面

## 技术栈

- **语言**：C++20
- **编译器**：clang
- **构建**：CMake(3.20) & Ninja
- **包管理**：vcpkg
- **网络**：SFML Network
- **序列化**：nlohmann/json
- **测试**：Google Test

---

## 快速开始

### 安装 vcpkg

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install
```

### 安装依赖
```bash
./vcpkg/vcpkg install
```

### 构建
1. debug 模式
```bash
cmake --preset debug
cmake --build --preset debug
./cmake-build-debug/poker
```

2. release 模式
```bash
cmake --preset release
cmake --build --preset release
./cmake-build-release/poker
```

3. 测试
```bash
cmake --preset debug-with-tests
cmake --build --preset debug
ctest --preset test
```

# 下一步计划

- [ ] 累计 short all-in 重开规则（TDA 47）
- [ ] 恢复不变量 / fuzz 测试
- [ ] 序列化对齐 + 重写 `server_main` / `client_main`，打通局域网联机
- [ ] 加入简单 AI（随机/半随机策略）
- [ ] 控制台 UI 增强（彩色输出）
- [ ] 图形界面（Qt 或 SFML）
