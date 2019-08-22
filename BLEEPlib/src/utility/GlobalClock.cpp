#include "GlobalClock.h"
#include <iostream>
#include <string.h>

using namespace libBLEEP;

long libBLEEP::globalclock_starttime = 946684800; // hardcoded for shadow start time

// buf needs to store 30 characters
static int timespec2str(char *buf, uint len, struct timespec *ts) {
    uint ret;
    struct tm t;

    tzset();
    if (localtime_r(&(ts->tv_sec), &t) == NULL)
        return 1;

    ret = strftime(buf, len, "%F %T", &t);
    if (ret == 0)
        return 2;
    len -= ret - 1;

    ret = snprintf(&buf[strlen(buf)], len, ".%09ld", ts->tv_nsec);
    if (ret >= len)
        return 3;

    return 0;
}

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

void libBLEEP::PrintTimeDiff(const char* prefix, const struct timespec& start, const struct timespec& end) {
    double milliseconds = end.tv_nsec >= start.tv_nsec
                        ? (end.tv_nsec - start.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec) * 1e3
                        : (start.tv_nsec - end.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec - 1) * 1e3;
    std::cout << prefix << ": " << milliseconds << "milliseconds" << "\n";
}

void libBLEEP::PrintTimespec(const char* prefix) {
    struct timespec tspec;
    clock_gettime(CLOCK_MONOTONIC, &tspec);
    const uint TIME_FMT = strlen("2012-12-31 12:59:59.123456789") + 1;
    char timestr[TIME_FMT];
    if (timespec2str(timestr, sizeof(timestr), &tspec) != 0) {
        std::cout << "timespec2str failed" << "\n";
        exit(-1);
    }
    std::cout << timestr << ":" << prefix << "\n";
    return;
}
