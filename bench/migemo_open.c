// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// profile_migemo_open.c - Profile migemo_open (loading the dictionary)
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

#define NUM_TRIAL 25

#include <stdio.h>

#include "bench_common.h"
#include "migemo.h"

#ifndef DICTDIR
# define DICTDIR "../../dict"
#endif

#define CLOCK2SEC(t) ((double)(t) / (double)CLOCKS_PER_SEC)

int
main(int argc, char **argv)
{
    int trial = NUM_TRIAL;
    migemo *mo;
    bench_time_t sum_open = 0, sum_close = 0;
    for (int i = 0; i < trial; i++)
    {
        TIME_MEASURE_ADD(sum_open)
        {
            mo = migemo_open(DICTDIR "/" MIGEMO_DICT_FILENAME);
        }
        TIME_MEASURE_ADD(sum_close) { migemo_close(mo); }
    }
    printf("Results:\n");
    printf("  migemo_open:  %.3f secs\n", time_to_sec(sum_open));
    printf("  migemo_close: %.3f secs\n", time_to_sec(sum_close));
    return 0;
}
