#pragma once

#include <string_view>

namespace poker::logging {
class Logger {
public:
    virtual ~Logger() = default;
    virtual void debug(std::string_view) {}
    virtual void info(std::string_view) {}
    virtual void warn(std::string_view) {}
    virtual void error(std::string_view) {}
};
} // namespace poker::logging