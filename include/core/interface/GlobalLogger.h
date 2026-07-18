#pragma once
#include <functional>
#include <string_view>

namespace poker {

class GlobalLogger {
public:
    inline static std::function<void(std::string_view)> trace{};
    inline static std::function<void(std::string_view)> info{};
    inline static std::function<void(std::string_view)> debug{};
    inline static std::function<void(std::string_view)> warn{};
    inline static std::function<void(std::string_view)> error{};
    inline static std::function<void(std::string_view)> critical{};
};

} // namespace poker