#ifndef LOGMANAGER_H
#define LOGMANAGER_H

enum LOGGER_OPTION {
    LOGGER_OFF = 0,
    LOGGER_ON = 1,
};

namespace utility {
    extern LOGGER_OPTION logger_opt;
    LOGGER_OPTION GetLoggerOption();
}




#endif
