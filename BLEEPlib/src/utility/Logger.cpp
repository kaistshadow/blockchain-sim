#include "Logger.h"

#include <stdio.h>
#include <stdarg.h>

using namespace libBLEEP;

Logger libBLEEP::gLog;

bool Logger::shouldFilter(LogLevel level) {
    return (level > _loglevel) ? true : false;
}

int Logger::printf(LogLevel level, const char *format, ...) {
    if (shouldFilter(level))
        return 0;

    va_list vargs;
    va_start(vargs, format);
    int result = vprintf(format, vargs);
    va_end(vargs);
    return result;
}
