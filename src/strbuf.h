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
void strbuf_close(strbuf *sb);
size_t strbuf_extend(strbuf *sb, size_t req_len);
size_t strbuf_append(strbuf *sb, strbuf *q);
size_t strbuf_append_ch(strbuf *sb, unsigned char ch);
size_t strbuf_append_str(strbuf *sb, const unsigned char *s);
size_t strbuf_append_mem(strbuf *sb, const unsigned char *p, size_t len);

#ifdef __cplusplus
}
#endif

static inline size_t
strbuf_len(strbuf *sb)
{
    return sb->len;
}

static inline unsigned char *
strbuf_get(strbuf *sb)
{
    return sb->buf;
}

static inline void
strbuf_reset(strbuf *sb)
{
    sb->len = 0;
    sb->buf[0] = '\0';
}

static inline size_t
strbuf_add(strbuf *sb, unsigned char ch)
{
    if (sb->len + 2 <= sb->cap)
    {
        sb->buf[sb->len++] = ch;
        sb->buf[sb->len] = 0;
        return sb->len;
    }
    return strbuf_append_ch(sb, ch);
}
