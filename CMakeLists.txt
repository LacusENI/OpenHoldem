cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest CONFIG REQUIRED)

add_library(PokerCore
        include/Card.h
        include/HandEvaluator.h
        include/Deck.h
        include/Game.h
        src/Card.cpp
        src/Game.cpp
        src/HandEvaluator.cpp
)

target_include_directories(PokerCore PUBLIC include)

add_executable(poker_tests
        tests/test_card.cpp
        tests/test_hand.cpp
        tests/test_game.cpp
)

target_link_libraries(poker_tests PRIVATE
        PokerCore
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(poker_tests)