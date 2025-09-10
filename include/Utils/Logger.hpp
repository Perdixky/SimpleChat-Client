#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <sstream>

// Forward declaration for the initialization function
void initLogging();

// The old severity levels, for compatibility at call sites
enum SeverityLevel { trace, debug, info, warning, error, fatal };

// Mapping from old severity to spdlog level
inline spdlog::level::level_enum to_spdlog_level(SeverityLevel level) {
    switch (level) {
        case trace: return spdlog::level::trace;
        case debug: return spdlog::level::debug;
        case info: return spdlog::level::info;
        case warning: return spdlog::level::warn;
        case error: return spdlog::level::err;
        case fatal: return spdlog::level::critical;
    }
    return spdlog::level::off;
}

// A wrapper class to capture the stream and log it using spdlog
class LogStream {
public:
    LogStream(spdlog::level::level_enum level) : level_(level) {}

    ~LogStream() {
        // When the object is destroyed, log the captured message
        if (stream_.tellp() > 0) { // Only log if there is something to log
            spdlog::log(level_, "{}", stream_.str());
        }
    }

    template<typename T>
    LogStream& operator<<(const T& msg) {
        stream_ << msg;
        return *this;
    }

private:
    std::ostringstream stream_;
    spdlog::level::level_enum level_;
};

// The new LOG macro
#define LOG(severity) LogStream(to_spdlog_level(severity))