#pragma once
#include <functional>
#include <string_view>

namespace poker {

class GlobalLogger {
public:
    inline static std::function<void(std::string_view)> trace = [](std::string_view) {};
    inline static std::function<void(std::string_view)> info = [](std::string_view) {};
    inline static std::function<void(std::string_view)> debug = [](std::string_view) {};
    inline static std::function<void(std::string_view)> warn = [](std::string_view) {};
    inline static std::function<void(std::string_view)> error = [](std::string_view) {};
    inline static std::function<void(std::string_view)> critical = [](std::string_view) {};
};

} // namespace poker