// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_UTILITY_LOGGER_H_
#define BLEEPLIB_SRC_UTILITY_LOGGER_H_

#include <iostream>

namespace libBLEEP {
enum LogLevel {
    LOGLEVEL_UNSET,
    LOGLEVEL_ERROR,
    LOGLEVEL_CRITICAL,
    LOGLEVEL_MESSAGE,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG,
};

// refers to https://stackoverflow.com/a/40424272
// https://stackoverflow.com/a/17597747
class Logger {
 private:
    LogLevel _loglevel;

    std::ostream& _out_stream;

    bool shouldFilter(LogLevel level);

 public:
    // Constructor: User provides custom output stream, or uses default (std::cout).
    Logger(LogLevel level = LOGLEVEL_DEBUG, std::ostream& stream = std::cout):
    _loglevel(level), _out_stream(stream) {}

    void SetLogLevel(LogLevel level) { _loglevel = level; }
    // Templated operator>> that uses the std::ostream: Everything that has defined
    // an operator<< for the std::ostream (Everithing "printable" with std::cout
    // and its colleages) can use this function.
    // This operator is overloaded for compatibility issue.
    template<typename T>
        Logger& operator<< (const T& data) {
            // default log-level (message)
            if (shouldFilter(LOGLEVEL_MESSAGE))
                return *this;

            _out_stream << data;
            return *this;
        }

    int printf(LogLevel level, const char *format, ...);
};

extern Logger gLog;
}  // namespace libBLEEP

#endif  // BLEEPLIB_SRC_UTILITY_LOGGER_H_
