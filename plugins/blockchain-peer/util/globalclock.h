#ifndef GLOBALCLOCK_H
#define GLOBALCLOCK_H

#include <chrono>
#include <time.h>
namespace utility {
    /* extern time_t globalclock_start; */
    extern std::chrono::high_resolution_clock::time_point globalclock_start;
    double GetGlobalClock(); 
}







#endif
