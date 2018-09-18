#include "globalclock.h"

time_t utility::globalclock_start;

double utility::GetGlobalClock() {
    return difftime(time(0), utility::globalclock_start); 
}
