/* vim:set ts=8 sts=4 sw=4 tw=0 et: */
/*
 * profile_speed.c - Query speed profiler.
 *
 * Author:  MURAOKA Taro <koron.kaoriya@gmail.com>
 */

#define NUM_TRIAL 10

#include <stdio.h>
#include <time.h>
#include "migemo.h"

#ifndef DICTDIR
# define DICTDIR "../dict"
#endif

#define CLOCK2SEC(t)    ((double)(t) / (double)CLOCKS_PER_SEC)

    int
main(int argc, char** argv)
{
    migemo* pmig;
    char* ans;
    char key[2] = { '\0', '\0' };
    int i;
    clock_t clock_load = 0, clock_query = 0, clock_tmp = 0;

    printf("Start\n");
    clock_tmp = clock();
    pmig = migemo_open(DICTDIR "/migemo-dict");
    clock_load = clock() - clock_tmp;
    printf("Loaded\n");
    if (pmig != NULL)
    {
        clock_query = 0;
        for (i = 0; i < NUM_TRIAL; ++i)
        {
            printf("[%d] Progress... ", i);
            for (key[0] = 'a'; key[0] <= 'z'; ++key[0])
            {
                printf("%s", key);
                fflush(stdout);
                clock_tmp = clock();
                ans = migemo_query(pmig, key);
                migemo_release(pmig, ans);
                clock_query += clock() - clock_tmp;
            }
            printf("\n");
        }
        migemo_close(pmig);
    }
    printf("\n");
    printf("Results:\n");
    printf("  load: %.3f secs\n", CLOCK2SEC(clock_load));
    printf("  query: %.3f secs\n", CLOCK2SEC(clock_query));
    return 0;
}
