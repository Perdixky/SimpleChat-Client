#include "Utils/Logger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

void initLogging()
{
    // Create a color console logger.
    auto console = spdlog::stdout_color_mt("console");
    
    // Set the default logger to the console logger.
    spdlog::set_default_logger(console);

    // Set the format pattern. 
    // [%H:%M:%S.%f] - Timestamp
    // [%^%l%$] - Colorized log level
    // [%t] - Thread ID
    // %v - The actual log message
    spdlog::set_pattern("[%H:%M:%S.%f] [%^%l%$] [%t] %v");

    // Set the global logging level to trace.
    spdlog::set_level(spdlog::level::trace);
}



