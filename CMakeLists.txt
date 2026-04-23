cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest CONFIG REQUIRED)

add_library(poker_core

        include/models/Card.h
        include/models/Deck.h
        include/models/GameTypes.h
        include/models/Hand.h
        include/models/Player.h

        include/Game.h
        include/GameModel.h
        include/HandEvaluator.h
        include/IGameView.h
        include/ShowdownHandler.h

        src/core/models/Card.cpp
        src/core/models/Deck.cpp

        src/core/Game.cpp
        src/core/GameModel.cpp
        src/core/HandEvaluator.cpp
        src/core/ShowdownHandler.cpp
)

target_include_directories(poker_core PUBLIC include)

add_executable(open_holdem
        src/main.cpp
        src/ConsoleView.h
        src/ConsoleView.cpp
)

target_link_libraries(open_holdem PRIVATE
        poker_core
)

add_executable(poker_tests
        tests/MockDeck.h
        tests/MockGameView.h

        tests/card_test.cpp
        tests/hand_evaluator_test.cpp
        tests/game_model_test.cpp
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