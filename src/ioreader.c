// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// ioreader.c - A layer that transparently handles raw files and zstd.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zstd.h>

#include "ioreader.h"

struct ioreader
{
    FILE *fp;

    ZSTD_DCtx *dctx;
    ZSTD_inBuffer in;
    size_t cap;
    uint8_t buf[];
};

ioreader *
ioreader_open(const char *filename, const char *mode)
{
    size_t len = strlen(filename);
    bool is_zstd = len > 4 && strcmp(filename + len - 4, ".zst") == 0;

    FILE *fp = fopen(filename, is_zstd ? "rb" : mode);
    if (!fp)
        return NULL;

    size_t cap = is_zstd ? ZSTD_DStreamInSize() : 0;
    ioreader *r = calloc(1, sizeof(ioreader) + cap);
    if (!r)
    {
        fclose(fp);
        return NULL;
    }
    r->fp = fp;
    r->cap = cap;

    // TODO: setup zstd context

    return r;
}

void
ioreader_close(ioreader *r)
{
    if (!r)
        return;
    // TODO: free zstd context if available.
    fclose(r->fp);
    free(r);
}

size_t
ioreader_read(void *ptr, size_t size, size_t nmemb, ioreader *r)
{
    // TODO: support zstd
    return fread(ptr, size, nmemb, r->fp);
}

int
ioreader_eof(ioreader *r)
{
    if (!r)
        return 0;
    // TODO: support zstd
    return feof(r->fp);
}

int
ioreader_error(ioreader *r)
{
    if (!r)
        return 0;
    // TODO: support zstd
    return ferror(r->fp);
}
