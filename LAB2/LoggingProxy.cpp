#include "LoggingProxy.h"

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
    std::array<std::size_t, 4> g_counters{};
    bool g_enabled = true;
}  

const char* LoggingProxy::toString(LogSource s) {
    switch (s) {
    case LogSource::ENV:    return "ENV";
    case LogSource::OBJECT: return "OBJECT";
    case LogSource::MODULE: return "MODULE";
    case LogSource::CENTER: return "CENTER";
    }
    return "?";
}

void LoggingProxy::log(LogSource source, const std::string& message) {
    ++g_counters[static_cast<std::size_t>(source)];
    if (!g_enabled) return;
    std::cout << "[" << toString(source) << "] " << message << '\n';
}

void LoggingProxy::setEnabled(bool enabled) { g_enabled = enabled; }

std::size_t LoggingProxy::count(LogSource source) {
    return g_counters[static_cast<std::size_t>(source)];
}

std::size_t LoggingProxy::total() {
    std::size_t sum = 0;
    for (auto c : g_counters) sum += c;
    return sum;
}

std::string LoggingProxy::fmt(double value, int precision) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(precision) << value;
    return os.str();
}