cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

# 项目依赖

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED COMPONENTS Network)
find_package(nlohmann_json CONFIG REQUIRED)

# 业务核心
add_library(poker_core
        include/core/algo/chips_calc.h
        include/core/algo/eval_hand.h
        include/core/algo/round_state.h
        include/core/client/ClientModel.h
        include/core/client/ClientController.h
        include/core/client/ClientView.h
        include/core/common/Card.h
        include/core/common/Deck.h
        include/core/common/GameTypes.h
        include/core/event/Event.h
        include/core/event/EventBus.h
        include/core/server/BettingRound.h
        include/core/server/GameController.h
        include/core/server/GameModel.h
        include/core/server/PotManager.h
        include/core/server/SeatManager.h

        src/core/algo/chips_calc.cpp
        src/core/algo/eval_hand.cpp
        src/core/algo/round_state.cpp
        src/core/client/ClientController.cpp
        src/core/common/Card.cpp
        src/core/common/Deck.cpp
        src/core/common/GameTypes.cpp
        src/core/common/HandValue.cpp
        src/core/server/BettingRound.cpp
        src/core/server/GameController.cpp
        src/core/server/GameModel.cpp
        include/core/server/Server.h
        include/core/client/Client.h
)

target_include_directories(poker_core PUBLIC
        include/core/event
        include/core/algo
        include/core/client
        include/core/common
        include/core/server
)


# 程序 UI
add_library(poker_ui
        include/ui/ConsoleView.h
        include/ui/Formatter.h

        src/ui/ConsoleView.cpp
        src/ui/Formatter.cpp
)

target_include_directories(poker_ui PUBLIC
        include/ui
)

target_link_libraries(poker_ui PUBLIC
        poker_core
        spdlog::spdlog_header_only
)


# 客户端程序
add_executable(poker_client
        src/client.cpp
)

target_link_libraries(poker_client PRIVATE
        poker_core
        SFML::Network
        nlohmann_json::nlohmann_json
)


# 服务器程序
add_executable(poker_server
        src/server.cpp
)

target_link_libraries(poker_server PRIVATE
        poker_core
        SFML::Network
        nlohmann_json::nlohmann_json
)


# 测试
add_executable(poker_test
        test/include/MockDeck.h

        test/card_test.cpp
        test/eval_hand_test.cpp
        test/chips_calc_test.cpp
)

target_include_directories(poker_test PRIVATE
        test/include
)

target_link_libraries(poker_test PRIVATE
        poker_core
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(poker_test)