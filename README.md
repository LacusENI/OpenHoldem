# OpenHoldem

![C++](https://img.shields.io/badge/C++-20-blue)
![CMake](https://img.shields.io/badge/CMake-3.20-blue)
![Vcpkg](https://img.shields.io/badge/vcpkg-enabled-blue)
![Platform](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![Status](https://img.shields.io/badge/status-alpha-orange)
![Version](https://img.shields.io/badge/version-demo_v5-yellow)
![License](https://img.shields.io/badge/license-MIT-green)

一个基于 C++ 的德州扑克游戏，支持局域网联机对战。

> 当前状态：核心逻辑 + 网络联机已跑通，可进行单局对战。

## 功能特性
- [ ] 完整的德州扑克规则
- [x] 事件驱动的游戏状态机
- [x] 服务端 / 客户端网络架构
- [x] JSON 序列化（nlohmann/json）
- [ ] AI 对手
- [ ] 锦标赛模式
- [ ] 图形界面

## 技术栈

- **语言**：C++20
- **构建**：CMake
- **包管理**：vcpkg
- **网络**：SFML Network
- **序列化**：nlohmann/json
- **测试**：Google Test

---

## 快速开始

### 依赖安装

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install
```

### 编译
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
make
```

### 运行
```
./poker_server
./poker_client
./poker_test
```

# 下一步计划

- [ ] 实现 All-in 和边池计算
- [ ] 加入简单 AI（随机策略）
- [ ] 控制台 UI 增强（彩色输出）
- [ ] 图形界面（Qt 或 SFML）
