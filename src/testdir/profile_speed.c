// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// profile_speed.c - Query speed profiler.
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#define NUM_TRIAL 10

#include <stdio.h>
#include <time.h>

#include "migemo.h"
#include "migemo_struct.h"

#ifndef DICTDIR
# define DICTDIR "../../dict"
#endif

#define CLOCK2SEC(t) ((double)(t) / (double)CLOCKS_PER_SEC)

static clock_t
profile_queries(migemo *pmig, int trial)
{
    char key[2] = {'\0', '\0'};
    clock_t dur = 0;
    for (int i = 0; i < trial; ++i)
    {
        printf("[%d] Progress... ", i);
        for (key[0] = 'a'; key[0] <= 'z'; ++key[0])
        {
            printf("%s", key);
            fflush(stdout);
            clock_t start = clock();
            char *ans = migemo_query(pmig, key);
            migemo_release(pmig, ans);
            dur += clock() - start;
        }
        printf("\n");
    }
    return dur;
}

int
main(int argc, char **argv)
{
    migemo *pmig;
    clock_t clock_load = 0, clock_query = 0, clock_tmp = 0;

    printf("Loading\n");
    clock_tmp = clock();
    pmig = migemo_open(DICTDIR "/migemo-dict");
    clock_load = clock() - clock_tmp;
    mtree_print_stat(pmig->mtree, "mtree statistics");
    if (pmig != NULL)
    {
        printf("Quering\n");
        clock_query = profile_queries(pmig, NUM_TRIAL);
        migemo_close(pmig);
    }
    printf("\n");
    printf("Results:\n");
    printf("  load: %.3f secs\n", CLOCK2SEC(clock_load));
    printf("  query: %.3f secs\n", CLOCK2SEC(clock_query));
    return 0;
}
