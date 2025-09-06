#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

// Define severity levels
enum SeverityLevel
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

// The formatting logic for severity levels
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, SeverityLevel lvl)
{
    static const char* const str[] =
    {
        "trace",
        "debug",
        "info",
        "warning",
        "error",
        "fatal"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

// Define the logger macro
#define LOG(severity) BOOST_LOG_SEV(logger::get(), severity)

// Declare the logger instance and initialization function
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, src::severity_logger_mt<SeverityLevel>)

void initLogging();
