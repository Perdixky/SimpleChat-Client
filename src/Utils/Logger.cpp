#include "Utils/Logger.hpp"
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

void initLogging()
{
    logging::add_common_attributes();

    logging::add_console_log(
        std::cout,
        keywords::format =
        (
            expr::stream
                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                << " [" << expr::attr<SeverityLevel>("Severity") << "] "
                << "[" << expr::attr<logging::thread_id::native_type>("ThreadID") << "] "
                << expr::smessage
        )
    );

    // Filter logs to a certain severity level
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::trace
    );
}
