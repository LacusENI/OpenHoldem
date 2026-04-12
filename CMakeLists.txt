cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest CONFIG REQUIRED)

add_library(poker_core
        include/Card.h
        src/Card.cpp
        include/Deck.h
        include/HandEvaluator.h
        src/HandEvaluator.cpp
        include/Game.h
        src/Game.cpp
        include/GameModel.h
        src/GameModel.cpp
        include/ShowdownHandler.h
        src/ShowdownHandler.cpp
        include/GameView.h
        src/GameView.cpp
)

target_include_directories(poker_core PUBLIC include)

add_executable(open_holdem
        src/main.cpp
)

target_link_libraries(open_holdem PRIVATE
        poker_core
)

add_executable(poker_tests
        tests/card_test.cpp
        tests/hand_evaluator_test.cpp
        tests/game_test.cpp
        tests/game_model_test.cpp
        docs/showdown_handler_test.txt
)

target_link_libraries(poker_tests PRIVATE
        poker_core
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(poker_tests)