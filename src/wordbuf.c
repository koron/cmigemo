// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordbuf.h"

#define WORDBUF_DEFAULT_SIZE 64
#define WORDBUF_MAX_SIZE     (8 * 1024 * 1024)

int n_wordbuf_open = 0;  // for DEBUG
int n_wordbuf_close = 0; // for DEBUG

wordbuf *
wordbuf_open()
{
    wordbuf *p = (wordbuf *)malloc(sizeof(wordbuf));
    if (!p)
        return NULL;

    p->len = WORDBUF_DEFAULT_SIZE;
    p->buf = (unsigned char *)malloc(p->len);
    if (!p->buf)
    {
        free(p);
        return NULL;
    }
    p->last = 0;
    p->buf[0] = '\0';
    ++n_wordbuf_open; // for DEBUG
    return p;
}

void
wordbuf_close(wordbuf *p)
{
    if (p)
    {
        ++n_wordbuf_close; // for DEBUG
        free(p->buf);
        free(p);
    }
}

// wordbuf_extend(wordbuf * p, int req_len);
//	Expand the buffer. Returns 0 on error.
//	The caller decides whether to expand for performance.
size_t
wordbuf_extend(wordbuf *p, size_t req_len)
{
    if (req_len > WORDBUF_MAX_SIZE)
        return 0;

    size_t newlen = p->len * 2;
    unsigned char *newbuf;

    while (req_len > newlen)
        newlen *= 2;
    if (!(newbuf = (unsigned char *)realloc(p->buf, newlen)))
    {
        // fprintf(stderr, "wordbuf_add(): failed to extend buffer\n");
        return 0;
    }
    else
    {
        p->len = newlen;
        p->buf = newbuf;
        return req_len;
    }
}

size_t
wordbuf_last(wordbuf *p)
{
    return p->last;
}

size_t
wordbuf_cat(wordbuf *p, const unsigned char *sz)
{
    size_t len = 0;

    if (sz != NULL)
    {
        size_t l = strlen(sz);
        len = l < INT_MAX ? l : INT_MAX;
    }

    if (len > 0)
    {
        size_t newlen = (size_t)p->last + len + 1;

        if (newlen > p->len && !wordbuf_extend(p, newlen))
            return 0;
        memcpy(&p->buf[p->last], sz, len + 1);
        p->last += len;
    }
    return p->last;
}

size_t
wordbuf_write_bytes(wordbuf *buf, const unsigned char *p, size_t len)
{
    if (p != NULL && len > 0)
    {
        size_t newlen = buf->last + len + 1;
        if (newlen > buf->len && !wordbuf_extend(buf, newlen))
            return 0;
        memcpy(&buf->buf[buf->last], p, len);
        buf->last += len;
        buf->buf[buf->last] = '\0';
    }
    return buf->last;
}

unsigned char *
wordbuf_get(wordbuf *p)
{
    return p->buf;
}
