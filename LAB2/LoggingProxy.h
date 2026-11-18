#pragma once
#include <cstddef>
#include <string>

enum class LogSource { ENV = 0, OBJECT, MODULE, CENTER };

// Простой логгер без внешних зависимостей.
class LoggingProxy {
public:
    static void log(LogSource source, const std::string& message);
    static void setEnabled(bool enabled);
    static std::size_t count(LogSource source);
    static std::size_t total();
    static const char* toString(LogSource source);
    static std::string fmt(double value, int precision = 1);

private:
    LoggingProxy() = delete;
}; 