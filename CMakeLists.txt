cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

# 项目依赖

find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# 业务核心
add_library(poker_core
        include/core/math/chips_calc.h
        include/core/math/eval_hand.h
        include/core/value_object/Card.h
        include/core/value_object/Deck.h
        include/core/value_object/GameTypes.h
        include/core/BettingRound.h
        include/core/GameController.h
        include/core/GameModel.h
        include/core/PotManager.h
        include/core/RoundTracker.h
        include/core/SeatingManager.h
        include/core/ShowdownHandler.h

        src/core/math/chips_calc.cpp
        src/core/math/eval_hand.cpp
        src/core/value_object/Card.cpp
        src/core/value_object/Deck.cpp
        src/core/value_object/HandValue.cpp
        src/core/BettingRound.cpp
        src/core/GameController.cpp
        src/core/GameModel.cpp
        src/core/PotManager.cpp
        src/core/RoundTracker.cpp
        src/core/ShowdownHandler.cpp
)

target_include_directories(poker_core PUBLIC
        include/core/math
        include/core/value_object
        include/core
)


# 程序 UI
add_library(poker_ui
        include/ui/Formatter.h

        src/ui/Formatter.cpp
)

target_include_directories(poker_ui PUBLIC
        include/ui
)

target_link_libraries(poker_ui PUBLIC
        poker_core
        spdlog::spdlog_header_only
)


# 主程序
add_executable(open_holdem
        src/main.cpp
)

target_link_libraries(open_holdem PRIVATE
        poker_core
        poker_ui
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