cmake_minimum_required(VERSION 3.20)
project(OpenHoldem)

set(CMAKE_CXX_STANDARD 20)

# 项目依赖
find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SFML CONFIG REQUIRED COMPONENTS Network)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)


# 业务核心
add_library(poker_core
        include/core/common/Card.hpp
        include/core/common/Deck.hpp
        include/core/common/GameTypes.hpp
        include/core/common/HandValue.hpp
        include/core/common/Player.hpp
        include/core/common/PlayerSet.hpp

        include/core/algo/chips_calc.h
        include/core/algo/eval_hand.h
        src/core/algo/chips_calc.cpp
        src/core/algo/eval_hand.cpp
        
        include/core/client/ClientModel.h
        include/core/client/ClientController.h
        include/core/client/ClientView.h
        src/core/client/ClientController.cpp

        include/core/event/Event.hpp
        include/core/event/EventBus.hpp

        include/core/interface/ClientIoAdapter.h
        include/core/interface/GlobalLogger.h
        include/core/interface/ServerIoAdapter.h

        include/core/model/BettingRound.h
        include/core/model/CardsManager.h
        include/core/model/ChipsManager.h
        include/core/model/SeatManager.h
        src/core/model/BettingRound.cpp
        src/core/model/CardsManager.cpp
        src/core/model/ChipsManager.cpp
        src/core/model/SeatManager.cpp
        
        include/core/server/GameController.h
        include/core/server/GameModel.h
        include/core/server/GameSession.h
        src/core/server/GameController.cpp
        src/core/server/GameModel.cpp
        include/core/algo/side_pots.h        
)

target_include_directories(poker_core PUBLIC
        include/core/algo
        include/core/client
        include/core/common
        include/core/event
        include/core/interface
        include/core/model
        include/core/server
)

target_link_libraries(poker_core PUBLIC
        magic_enum::magic_enum
)


# 测试业务核心逻辑的程序
add_executable(poker
        src/main.cpp
)

target_link_libraries(poker PRIVATE
        poker_core
)


# 应用层
add_library(application
        include/logging/EventLogger.hpp

        include/network/ClientNetwork.h
        include/network/ServerNetwork.h

        include/serialization/EventSerializer.h
        src/serialization/EventSerializer.cpp

        include/serialization/GeneralSerializer.hpp
        include/network/MockNetwork.h
        src/network/ClientNetwork.cpp
        src/network/ServerNetwork.cpp
)

target_include_directories(application PUBLIC
        include/logging
        include/network
        include/serialization
)

target_link_libraries(application PUBLIC
        poker_core
        magic_enum::magic_enum
        spdlog::spdlog_header_only
        nlohmann_json::nlohmann_json
        SFML::Network
)


# 表现层
add_library(presentation
        include/presentation/ConsoleView.h
        include/presentation/Formatter.h

        src/presentation/ConsoleView.cpp
        src/presentation/Formatter.cpp
)

target_include_directories(presentation PUBLIC
        include/presentation
)

target_link_libraries(presentation PUBLIC
        poker_core
        spdlog::spdlog_header_only
)


# 测试
add_executable(poker_test
        test/include/MockDeck.hpp
        test/betting_round_test.cpp
        test/card_test.cpp
        test/cards_manager_test.cpp
        test/chips_calc_test.cpp
        test/chips_manager_test.cpp
        test/deck_test.cpp
        test/eval_hand_test.cpp
        test/game_model_test.cpp
        test/game_types_test.cpp
        test/global_logger_test.cpp
        test/invariants_test.cpp
        test/seat_manager_test.cpp
        test/side_pots_test.cpp
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