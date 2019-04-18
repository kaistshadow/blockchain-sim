#include "GlobalClock.h"

using namespace libBLEEP;

long libBLEEP::globalclock_starttime = 946684800; // hardcoded for shadow start time

double libBLEEP::GetGlobalClock() {
    // auto now = std::chrono::high_resolution_clock::now();
    // double elapsed_milli = std::chrono::duration_cast<std::chrono::milliseconds>(now - utility::globalclock_start).count();
    // return elapsed_milli/1000.0;

    /* use time */
    // char buff[100];
    // time_t now2 = time(0);
    // unsigned long long second = now2 - 946684800;
    // strftime (buff, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now2));
    // printf("%s\n", buff);
    // printf("%llu seconds\n", second);

    /* use gettimeofday */
    // struct timeval tv;
    // gettimeofday(&tv, NULL);
    // unsigned long long milliiseconds = 
    //     (unsigned long long)(tv.tv_sec - 946684800) * 1000 +
    //     (unsigned long long)(tv.tv_usec) / 1000;
    // printf("%f second passed\n", (double)milliiseconds/1000);
    
    /* use clock_gettime */
    time_t s;
    struct timespec spec;
    long ms;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    s = spec.tv_sec;
    ms = spec.tv_nsec / 1000000; 
    s -= globalclock_starttime;
    double curtime = (double)s + (double)ms/1000;

    return curtime;
}

unsigned long libBLEEP::GetCurrentTime() {
    unsigned long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();
    return milliseconds_since_epoch;
}
