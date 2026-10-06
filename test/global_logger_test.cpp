#include <gtest/gtest.h>

#include <string_view>

#include "GlobalLogger.h"

using namespace poker;

// 未赋值时默认应为空操作，而不是抛 std::bad_function_call
TEST(GlobalLoggerTest, DefaultIsNoOpAndDoesNotThrow) {
    EXPECT_NO_THROW(GlobalLogger::trace("t"));
    EXPECT_NO_THROW(GlobalLogger::info("i"));
    EXPECT_NO_THROW(GlobalLogger::debug("d"));
    EXPECT_NO_THROW(GlobalLogger::warn("w"));
    EXPECT_NO_THROW(GlobalLogger::error("e"));
    EXPECT_NO_THROW(GlobalLogger::critical("c"));
}

// 赋值后可被正常调用，切回默认仍安全
TEST(GlobalLoggerTest, CanBeOverridden) {
    int calls = 0;
    auto original = GlobalLogger::info;
    GlobalLogger::info = [&calls](std::string_view) { ++calls; };

    GlobalLogger::info("hello");
    EXPECT_EQ(1, calls);

    GlobalLogger::info = original;
    EXPECT_NO_THROW(GlobalLogger::info("restored"));
}
