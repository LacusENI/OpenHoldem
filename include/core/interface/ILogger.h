#pragma once
#include <string>

namespace poker::log {
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void trace(const std::string& msg) = 0;
    virtual void debug(const std::string& msg) = 0;
    virtual void info(const std::string& msg) = 0;
    virtual void warn(const std::string& msg) = 0;
    virtual void error(const std::string& msg) = 0;
    virtual void critical(const std::string& msg) = 0;
};

class NullLogger final : public ILogger {
public:
    void trace(const std::string& msg) override {}
    void debug(const std::string& msg) override {}
    void info(const std::string& msg) override {}
    void warn(const std::string& msg) override {}
    void error(const std::string& msg) override {}
    void critical(const std::string& msg) override {}
};
} // namespace poker::log
