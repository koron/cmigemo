// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// profile_migemo_open.c - Profile migemo_open (loading the dictionary)
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

#define NUM_TRIAL 25

#include <stdio.h>
#include <time.h>

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
    clock_t sum_open = 0, sum_close = 0;
    for (int i = 0; i < trial; i++)
    {
        clock_t t0 = clock();
        mo = migemo_open(DICTDIR "/" MIGEMO_DICT_FILENAME);
        clock_t t1 = clock();
        migemo_close(mo);
        clock_t t2 = clock();
        sum_open += t1 - t0;
        sum_close += t2 - t1;
    }
    printf("Results:\n");
    printf("  migemo_open:  %.3f secs\n", CLOCK2SEC(sum_open));
    printf("  migemo_close: %.3f secs\n", CLOCK2SEC(sum_close));
    return 0;
}
