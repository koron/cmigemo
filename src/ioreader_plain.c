// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// ioreader_plain.c - A layer that transparently handles raw files and zstd.

#include <stdio.h>
#include <stdlib.h>

#include "ioreader.h"

struct ioreader
{
    FILE *fp;
};

ioreader *
ioreader_open(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (!fp)
        return NULL;

    ioreader *r = calloc(1, sizeof(ioreader));
    if (!r)
    {
        fclose(fp);
        return NULL;
    }
    r->fp = fp;
    return r;
}

void
ioreader_close(ioreader *r)
{
    if (!r)
        return;
    fclose(r->fp);
    free(r);
}

size_t
ioreader_read(void *ptr, size_t size, size_t nmemb, ioreader *r)
{
    if (!r || size == 0 || nmemb == 0)
        return 0;
    return fread(ptr, size, nmemb, r->fp);
}

int
ioreader_eof(ioreader *r)
{
    if (!r)
        return 0;
    return feof(r->fp);
}

int
ioreader_error(ioreader *r)
{
    if (!r)
        return 0;
    return ferror(r->fp);
}
