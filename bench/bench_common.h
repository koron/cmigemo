// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// bench_common.h -
//

#pragma once

#include <stdint.h>
#include <time.h>

typedef uint64_t bench_time_t;

#if defined(_MSC_VER)
# include <windows.h>

static inline bench_time_t
time_now(void)
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return (bench_time_t)count.QuadPart;
}

static inline double
time_to_sec(bench_time_t duration)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (double)duration / (double)freq.QuadPart;
}

#else

static inline bench_time_t
time_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (bench_time_t)((uint64_t)ts.tv_sec * 1000000000ULL
                          + (uint64_t)ts.tv_nsec);
}

static inline double
time_to_sec(bench_time_t duration)
{
    return (double)duration / 1e9;
}

#endif

static inline bench_time_t
time_since(bench_time_t start)
{
    return time_now() - start;
}

#define TIME_MEASURE_ADD(dur)                                                  \
    for (bench_time_t _start = 0, _done = 0;                                   \
            !_done && (_start = time_now(), 1);                                \
            (dur) += time_since(_start), _done = 1)
