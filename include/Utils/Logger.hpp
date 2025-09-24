#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <format>
#include <source_location>
#include <string>
#include <cstdlib>
// Prevent macro 'log' below from breaking standard declarations
#include <cmath>
#include <complex>

// Forward declaration for the initialization function
void initLogging();

// The old severity levels, for compatibility at call sites
enum SeverityLevel { trace, debug, info, warning, error, fatal };

// Mapping from old severity to spdlog level
inline spdlog::level::level_enum to_spdlog_level(SeverityLevel level) {
  switch (level) {
  case trace:
    return spdlog::level::trace;
  case debug:
    return spdlog::level::debug;
  case info:
    return spdlog::level::info;
  case warning:
    return spdlog::level::warn;
  case error:
    return spdlog::level::err;
  case fatal:
    return spdlog::level::critical;
  }
  return spdlog::level::off;
}

template <typename... Args>
inline void log(SeverityLevel level, std::format_string<Args...> fmt,
                Args &&...args) {
  spdlog::log(to_spdlog_level(level),
              std::format(fmt, std::forward<Args>(args)...));
}

// Macro-backed implementation to enrich error/fatal with source_location
namespace UtilsLogDetail {
template <typename... Args>
inline void log_with_location(SeverityLevel level,
                              std::source_location loc,
                              std::format_string<Args...> fmt,
                              Args &&...args) {
  std::string msg = std::format(fmt, std::forward<Args>(args)...);
  if (level == error || level == fatal) {
    auto prefix = std::format("file: {}({}:{}) `{}`: ", loc.file_name(),
                              loc.line(), loc.column(), loc.function_name());
    msg = prefix + msg;
  }
  spdlog::log(to_spdlog_level(level), msg);
  if (level == fatal) {
    if (auto logger = spdlog::default_logger()) {
      logger->flush();
    }
    std::abort();
  }
}
} // namespace UtilsLogDetail

// Define macro to capture source_location automatically at call-sites.
// Disable by defining LOG_NO_MACRO before including this header.
#ifndef LOG_NO_MACRO
#define log(level, ...) \
  ::UtilsLogDetail::log_with_location((level), std::source_location::current(), __VA_ARGS__)
#endif
