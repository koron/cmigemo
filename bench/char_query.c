// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// profile_char_query.c - Profile single-character queries
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#define NUM_TRIAL 50

#include <stdio.h>

#include "bench_common.h"
#include "migemo.h"
#include "migemo_struct.h"

static bench_time_t
profile_char_query(migemo *mo, int trial)
{
    char key[2] = {'\0', '\0'};
    bench_time_t dur = 0;

    for (int i = 0; i < trial; ++i)
    {
        for (key[0] = 'a'; key[0] <= 'z'; ++key[0])
        {
            TIME_MEASURE_ADD(dur)
            {
                char *ans = migemo_query(mo, key);
                migemo_release(mo, ans);
            }
        }
    }
    return dur;
}

int
main(int argc, char **argv)
{
    migemo *mo;
    bench_time_t dur_load = 0;
    bench_time_t dur_query = 0;
    bench_time_t dur_switch = 0;
    bench_time_t dur_query2 = 0;

    TIME_MEASURE_ADD(dur_load)
    {
        mo = migemo_open(DICTDIR "/" MIGEMO_DICT_FILENAME);
    }
    if (!mo)
        return 1;

    dur_query = profile_char_query(mo, NUM_TRIAL);

    TIME_MEASURE_ADD(dur_switch)
    {
        migemo_switch_sdict(mo, MIGEMO_SDICT_PRESERVE_MDICT);
    }

    dur_query2 = profile_char_query(mo, NUM_TRIAL);

    printf("Results:\n");
    printf("  load          : %.9f secs\n", time_to_sec(dur_load));
    printf("  query (mdict) : %.9f secs\n", time_to_sec(dur_query));
    printf("  switch        : %.9f secs\n", time_to_sec(dur_switch));
    printf("  query (sdict) : %.9f secs\n", time_to_sec(dur_query2));

    migemo_close(mo);
    return 0;
}
