// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordbuf.h"

#define WORDLEN_DEF 64

int n_wordbuf_open = 0;  // for DEBUG
int n_wordbuf_close = 0; // for DEBUG

wordbuf_p
wordbuf_open()
{
    wordbuf_p p = (wordbuf_p)malloc(sizeof(wordbuf_t));

    if (p)
    {
        ++n_wordbuf_open; // for DEBUG
        p->len = WORDLEN_DEF;
        p->buf = (unsigned char *)malloc(p->len);
        p->last = 0;
        p->buf[0] = '\0';
    }
    return p;
}

void
wordbuf_close(wordbuf_p p)
{
    if (p)
    {
        ++n_wordbuf_close; // for DEBUG
        free(p->buf);
        free(p);
    }
}

// wordbuf_extend(wordbuf_p p, int req_len);
//	Expand the buffer. Returns 0 on error.
//	The caller decides whether to expand for performance.
int
wordbuf_extend(wordbuf_p p, int req_len)
{
    int newlen = p->len * 2;
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

int
wordbuf_last(wordbuf_p p)
{
    return p->last;
}

int
wordbuf_cat(wordbuf_p p, const unsigned char *sz)
{
    int len = 0;

    if (sz != NULL)
    {
        size_t l = strlen(sz);
        len = l < INT_MAX ? (int)l : INT_MAX;
    }

    if (len > 0)
    {
        int newlen = p->last + len + 1;

        if (newlen > p->len && !wordbuf_extend(p, newlen))
            return 0;
        memcpy(&p->buf[p->last], sz, len + 1);
        p->last = p->last + len;
    }
    return p->last;
}

int
wordbuf_write_bytes(wordbuf_p buf, const unsigned char *p, size_t len)
{
    if (p != NULL && len > 0)
    {
        int newlen = buf->last + (int)len + 1;
        if (newlen > buf->len && !wordbuf_extend(buf, newlen))
            return 0;
        memcpy(&buf->buf[buf->last], p, len);
        buf->last = buf->last + (int)len;
        buf->buf[buf->last] = '\0';
    }
    return buf->last;
}

unsigned char *
wordbuf_get(wordbuf_p p)
{
    return p->buf;
}
