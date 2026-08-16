// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// strbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strbuf.h"

#define STRBUF_DEFAULT_SIZE 64
#define STRBUF_MAX_SIZE     (8 * 1024 * 1024)

strbuf *
strbuf_open()
{
    strbuf *sb = (strbuf *)malloc(sizeof(strbuf));
    if (!sb)
        return NULL;

    sb->cap = STRBUF_DEFAULT_SIZE;
    sb->buf = (unsigned char *)malloc(sb->cap);
    if (!sb->buf)
    {
        free(sb);
        return NULL;
    }
    sb->len = 0;
    sb->buf[0] = '\0';
    return sb;
}

void
strbuf_close(strbuf *sb)
{
    if (sb)
    {
        free(sb->buf);
        free(sb);
    }
}

// strbuf_extend(strbuf * sb, int req_len);
//	Expand the buffer. Returns 0 on error.
//	The caller decides whether to expand for performance.
size_t
strbuf_extend(strbuf *sb, size_t req_len)
{
    if (req_len > STRBUF_MAX_SIZE)
        return 0;

    size_t newlen = sb->cap * 2;
    while (req_len > newlen)
        newlen *= 2;

    unsigned char *newbuf;
    if (!(newbuf = (unsigned char *)realloc(sb->buf, newlen)))
        return 0;
    sb->cap = newlen;
    sb->buf = newbuf;
    return req_len;
}

size_t
strbuf_append_ch(strbuf *sb, unsigned char ch)
{
    if (!strbuf_extend(sb, sb->len + 2))
        return 0;
    return strbuf_add(sb, ch);
}

size_t
strbuf_append(strbuf *sb, strbuf *q)
{
    return strbuf_append_mem(sb, q->buf, q->len);
}

size_t
strbuf_append_str(strbuf *sb, const unsigned char *s)
{
    size_t len = 0;

    if (s != NULL)
    {
        size_t l = strlen(s);
        len = l < INT_MAX ? l : INT_MAX;
    }

    if (len > 0)
    {
        size_t newlen = (size_t)sb->len + len + 1;

        if (newlen > sb->cap && !strbuf_extend(sb, newlen))
            return 0;
        memcpy(&sb->buf[sb->len], s, len + 1);
        sb->len += len;
    }
    return sb->len;
}

size_t
strbuf_append_mem(strbuf *sb, const unsigned char *p, size_t len)
{
    if (p != NULL && len > 0)
    {
        size_t newlen = sb->len + len + 1;
        if (newlen > sb->cap && !strbuf_extend(sb, newlen))
            return 0;
        memcpy(&sb->buf[sb->len], p, len);
        sb->len += len;
        sb->buf[sb->len] = '\0';
    }
    return sb->len;
}

size_t
strbuf_truncate(strbuf *sb, size_t len)
{
    if (len >= sb->len)
        return sb->len;
    sb->buf[len] = '\0';
    sb->len = len;
    return len;
}
