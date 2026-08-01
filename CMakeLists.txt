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

        include/core/algo/chips_calc.h
        include/core/algo/eval_hand.h
        include/core/algo/round_state.h
        src/core/algo/chips_calc.cpp
        src/core/algo/eval_hand.cpp
        src/core/algo/round_state.cpp

        include/core/client/ClientModel.h
        include/core/client/ClientController.h
        include/core/client/ClientView.h
        src/core/client/ClientController.cpp

        include/core/event/Event.hpp
        include/core/event/EventBus.hpp

        include/core/interface/ClientIoAdapter.h
        include/core/interface/GlobalLogger.h
        include/core/interface/ServerIoAdapter.h

        include/core/model/ActionFlow.h
        include/core/model/BettingManager.h
        include/core/model/CardsManager.h
        include/core/model/ChipsManager.h
        include/core/model/PlayerSet.h
        include/core/model/PotManager.h
        include/core/model/SeatManager.h
        src/core/model/ActionFlow.cpp
        src/core/model/BettingManager.cpp
        src/core/model/CardsManager.cpp
        src/core/model/ChipsManager.cpp
        src/core/model/PlayerSet.cpp
        src/core/model/SeatManager.cpp
        

        include/core/server/BettingRound.h
        include/core/server/GameController.h
        include/core/server/GameModel.h
        src/core/server/BettingRound.cpp
        src/core/server/GameController.cpp
        src/core/server/GameModel.cpp
        
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


# 客户端程序
add_executable(poker_client
        src/client_main.cpp
)

target_link_libraries(poker_client PRIVATE
        poker_core
        application
        presentation
)


# 服务器程序
add_executable(poker_server
        src/server_main.cpp
)

target_link_libraries(poker_server PRIVATE
        poker_core
        application
)


# UI 测试
add_executable(ui_test
        ui_test/ui_test_main.cpp
        ui_test/table_test.cpp
        ui_test/ui_test.h
)

target_include_directories(ui_test PUBLIC
        ui_test.h
)

target_link_libraries(ui_test PRIVATE
        poker_core
        application
        presentation
)


# 测试
add_executable(poker_test
        test/include/MockDeck.hpp

        test/action_flow_test.cpp
        test/card_test.cpp
        test/eval_hand_test.cpp
        test/chips_calc_test.cpp
        test/seat_manager_test.cpp
        test/chips_manager_test.cpp
        test/cards_manager_test.cpp
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