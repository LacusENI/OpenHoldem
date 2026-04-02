cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest CONFIG REQUIRED)

add_library(PokerCore
        include/Card.h
        include/HandEvaluator.h
        include/Deck.h
        src/Card.cpp
        src/HandEvaluator.cpp
        include/Game.h
        src/main.cpp
        src/Game.cpp
)

target_include_directories(PokerCore PUBLIC include)

add_executable(poker_tests
        tests/test_card.cpp
        tests/test_hand.cpp
        tests/test_game.cpp
)

target_link_libraries(poker_tests PRIVATE PokerCore GTest::gtest GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(poker_tests)