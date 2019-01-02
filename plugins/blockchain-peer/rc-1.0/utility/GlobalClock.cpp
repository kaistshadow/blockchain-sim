#include "GlobalClock.h"

std::chrono::high_resolution_clock::time_point utility::globalclock_start;

double utility::GetGlobalClock() {
    auto now = std::chrono::high_resolution_clock::now();
    double elapsed_milli = std::chrono::duration_cast<std::chrono::milliseconds>(now - utility::globalclock_start).count();
    return elapsed_milli/1000.0;

    // return difftime(time(0), utility::globalclock_start); 
}
