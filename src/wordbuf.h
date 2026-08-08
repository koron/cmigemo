// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordbuf.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

typedef struct wordbuf wordbuf;
struct wordbuf
{
    int len;  // amount of memory allocated to buf
    int last; // length of the string actually stored in buf
    unsigned char *buf;
};

extern int n_wordbuf_open;
extern int n_wordbuf_close;

#define wordbuf_len(w) wordbuf_last(w)
#define WORDBUF_GET(w) ((w)->buf)
#define WORDBUF_LEN(w) ((w)->last)

#ifdef __cplusplus
extern "C" {
#endif

wordbuf *wordbuf_open();
void wordbuf_close(wordbuf *p);
int wordbuf_extend(wordbuf *p, int len);
int wordbuf_last(wordbuf *p);
int wordbuf_cat(wordbuf *p, const unsigned char *sz);
int wordbuf_write_bytes(wordbuf *buf, const unsigned char *p, size_t len);
unsigned char *wordbuf_get(wordbuf *p);

#ifdef __cplusplus
}
#endif

static inline void
wordbuf_reset(wordbuf *p)
{
    p->last = 0;
    p->buf[0] = '\0';
}

static inline int
wordbuf_add(wordbuf *p, unsigned char ch)
{
    int newlen = p->last + 2;
    if (newlen > p->len)
        if (!wordbuf_extend(p, newlen))
            return 0;

    p->buf[p->last++] = ch;
    p->buf[p->last] = 0;
    return p->last;
}
