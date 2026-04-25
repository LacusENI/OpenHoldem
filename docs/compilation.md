# 编译说明

## 清单

- C++20
- vcpkg
- cmake (>= 3.20)

## 安装 vcpkg

假设你的 vcpkg 安装在 `项目目录/vcpkg`:
```bash
git clone https://github.com/microsoft/vcpkg.git ./vcpkg
cd vcpkg
./bootstrap-vcpkg.sh
```

## 编译

假设你的 vcpkg 安装在 `项目目录/vcpkg`:
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
make
```

## 运行

运行主程序:
```bash
build/open_holdem
```
运行测试:
```bash
build/poker_test
```