// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// ioreader_zstd.c - A layer that transparently handles raw files and zstd.

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

    unsigned zstd_err;
    size_t zstd_last;

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

    if (!is_zstd)
        return r;

    // Setup zstd context
    r->dctx = ZSTD_createDCtx();
    if (!r->dctx)
    {
        free(r);
        fclose(fp);
        return NULL;
    }

    return r;
}

void
ioreader_close(ioreader *r)
{
    if (!r)
        return;
    if (r->dctx)
        ZSTD_freeDCtx(r->dctx);
    fclose(r->fp);
    free(r);
}

size_t
ioreader_read(void *ptr, size_t size, size_t nmemb, ioreader *r)
{
    if (!r || size == 0 || nmemb == 0)
        return 0;

    if (!r->dctx)
        return fread(ptr, size, nmemb, r->fp);

    ZSTD_outBuffer out = {
            .dst = ptr,
            .size = size * nmemb,
            .pos = 0,
    };

    while (out.pos < out.size)
    {
        if (r->in.pos == r->in.size)
        {
            if (feof(r->fp) && r->zstd_last == 0)
                break;
            size_t read_bytes = fread(r->buf, 1, r->cap, r->fp);
            if (read_bytes == 0)
                break;
            r->in.src = r->buf;
            r->in.size = read_bytes;
            r->in.pos = 0;
        }

        r->zstd_last = ZSTD_decompressStream(r->dctx, &out, &r->in);
        r->zstd_err = ZSTD_isError(r->zstd_last);
        if (r->zstd_err)
        {
            return 0;
        }
    }

    return out.pos / size;
}

int
ioreader_eof(ioreader *r)
{
    if (!r)
        return 0;
    if (!r->dctx)
        return feof(r->fp);
    return feof(r->fp) && (r->in.pos == r->in.size) && r->zstd_last == 0;
}

int
ioreader_error(ioreader *r)
{
    if (!r)
        return 0;
    if (!r->dctx)
        return ferror(r->fp);

    int err = ferror(r->fp);
    if (err != 0)
        return err;
    return r->zstd_err;
}
