cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

# 项目依赖

find_package(GTest CONFIG REQUIRED)


# 业务核心
add_library(poker_core
        include/core/value_object/Card.h
        include/core/value_object/Deck.h
        include/core/value_object/GameTypes.h
        include/core/BettingRound.h
        include/core/Game.h
        include/core/GameModel.h
        include/core/HandEvaluator.h
        include/core/IGameView.h
        include/core/PlayerSet.h
        include/core/ShowdownHandler.h

        src/core/value_object/Card.cpp
        src/core/value_object/Deck.cpp
        src/core/value_object/HandValue.cpp
        src/core/BettingRound.cpp
        src/core/Game.cpp
        src/core/GameModel.cpp
        src/core/HandEvaluator.cpp
        src/core/PlayerSet.cpp
        src/core/ShowdownHandler.cpp
)

target_include_directories(poker_core PUBLIC
        include/core/value_object
        include/core
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
        test/include/MockGameView.h

        test/card_test.cpp
        test/hand_evaluator_test.cpp
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