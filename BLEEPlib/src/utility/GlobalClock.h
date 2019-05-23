#ifndef GLOBALCLOCK_H
#define GLOBALCLOCK_H

#include <chrono>
#include <time.h>
namespace libBLEEP {
    /* extern time_t globalclock_start; */
    extern long globalclock_starttime;
    double GetGlobalClock();
    unsigned long GetCurrentTime();

    void PrintTimeDiff(const char* prefix, const struct timespec& start, const struct timespec& end);
}




#endif
