// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

typedef struct wordbuf wordbuf;
struct wordbuf
{
    size_t cap; // amount of memory allocated to buf
    size_t len; // length of the string actually stored in buf
    unsigned char *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

wordbuf *wordbuf_open();
void wordbuf_close(wordbuf *p);
size_t wordbuf_extend(wordbuf *p, size_t req_len);
size_t wordbuf_append(wordbuf *p, wordbuf *q);
size_t wordbuf_cat(wordbuf *p, const unsigned char *sz);
size_t wordbuf_write_bytes(wordbuf *buf, const unsigned char *p, size_t len);

#ifdef __cplusplus
}
#endif

static inline size_t
wordbuf_len(wordbuf *p)
{
    return p->len;
}

static inline unsigned char *
wordbuf_get(wordbuf *p)
{
    return p->buf;
}

static inline void
wordbuf_reset(wordbuf *p)
{
    p->len = 0;
    p->buf[0] = '\0';
}

static inline size_t
wordbuf_add(wordbuf *p, unsigned char ch)
{
    size_t newlen = p->len + 2;
    if (newlen > p->cap)
        if (!wordbuf_extend(p, newlen))
            return 0;

    p->buf[p->len++] = ch;
    p->buf[p->len] = 0;
    return p->len;
}
