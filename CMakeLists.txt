cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest CONFIG REQUIRED)

add_library(PokerCore
        src/Card.cpp
        include/Card.h)

target_include_directories(PokerCore PUBLIC include)

add_executable(poker_tests
        src/Card.cpp
        tests/test_card.cpp)

target_link_libraries(poker_tests PRIVATE PokerCore GTest::gtest GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(poker_tests)