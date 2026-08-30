// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// gen_sdict.c - Generate sdict from migemo-dict

#include <stdio.h>
#include <stdlib.h>

#include "migemo.h"

int
main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "USAGE: gen_sdict {in migemo-dict} {out sdict}\n");
        exit(1);
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];

    migemo *mo = migemo_open(infile);
    if (mo == NULL)
    {
        fprintf(stderr, "ERROR: failed to load: %s\n", infile);
        exit(1);
    }

    if (migemo_switch_sdict(mo, MIGEMO_SDICT_RELEASE_MDICT) == 0)
    {
        fprintf(stderr, "ERROR: failed to convert the dictionary to sdict\n");
        return 1;
    }

    if (migemo_save_sdict(mo, outfile) == 0)
    {
        fprintf(stderr, "ERROR: failed to save sdict: %s\n", outfile);
        return 1;
    }

    return 0;
}
