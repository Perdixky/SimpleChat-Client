#include "Utils/Logger.hpp"
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <thread>

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

// Custom formatter for severity level with ANSI colors
void colorize_severity(logging::record_view const& rec, logging::formatting_ostream& strm)
{
    logging::value_ref<SeverityLevel> severity = logging::extract<SeverityLevel>("Severity", rec);
    if (severity)
    {
        switch (severity.get())
        {
            case trace:
                strm << "\033[37m"; // white
                break;
            case debug:
                strm << "\033[36m"; // cyan
                break;
            case info:
                strm << "\033[32m"; // green
                break;
            case warning:
                strm << "\033[33m"; // yellow
                break;
            case error:
                strm << "\033[31m"; // red
                break;
            case fatal:
                strm << "\033[35m"; // magenta
                break;
            default:
                break;
        }
    }

    // The operator<< for SeverityLevel is already defined in Logger.hpp
    strm << "[" << severity << "]";

    // Reset color
    strm << "\033[0m";
}

void initLogging()
{
    logging::add_common_attributes();

    logging::add_console_log(
        std::cout,
        keywords::format =
        (
            expr::stream
                // Time format updated, date part removed
                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                // Use the custom color formatter for severity
                << " " << expr::wrap_formatter(&colorize_severity) << " "
                << "[" << expr::attr<logging::thread_id>("ThreadID") << "] "
                << expr::smessage
        )
    );

    // Filter logs to a certain severity level
    logging::core::get()->set_filter
    (
        expr::attr<SeverityLevel>("Severity") >= trace
    );
}
