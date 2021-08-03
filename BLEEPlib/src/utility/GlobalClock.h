// "Copyright [2021] <kaistshadow>"

#ifndef BLEEPLIB_SRC_UTILITY_GLOBALCLOCK_H_
#define BLEEPLIB_SRC_UTILITY_GLOBALCLOCK_H_

#include <chrono>
#include <time.h>
namespace libBLEEP {
    /* extern time_t globalclock_start; */
    extern long globalclock_starttime;
    double GetGlobalClock();
    unsigned long GetCurrentTime();

    void PrintTimeDiff(const char* prefix, const struct timespec& start, const struct timespec& end);

    void PrintTimespec(const char* prefix);
}




#endif // BLEEPLIB_SRC_UTILITY_GLOBALCLOCK_H_
