// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// strbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

typedef struct strbuf strbuf;
struct strbuf
{
    size_t cap; // amount of memory allocated to buf
    size_t len; // length of the string actually stored in buf
    unsigned char *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

strbuf *strbuf_open();
void strbuf_close(strbuf *p);
size_t strbuf_extend(strbuf *p, size_t req_len);
size_t strbuf_append(strbuf *p, strbuf *q);
size_t strbuf_append_ch(strbuf *p, unsigned char ch);
size_t strbuf_cat(strbuf *p, const unsigned char *sz);
size_t strbuf_write_bytes(strbuf *buf, const unsigned char *p, size_t len);

#ifdef __cplusplus
}
#endif

static inline size_t
strbuf_len(strbuf *p)
{
    return p->len;
}

static inline unsigned char *
strbuf_get(strbuf *p)
{
    return p->buf;
}

static inline void
strbuf_reset(strbuf *p)
{
    p->len = 0;
    p->buf[0] = '\0';
}

static inline size_t
strbuf_add(strbuf *p, unsigned char ch)
{
    if (p->len + 2 <= p->cap)
    {
        p->buf[p->len++] = ch;
        p->buf[p->len] = 0;
        return p->len;
    }
    return strbuf_append_ch(p, ch);
}
