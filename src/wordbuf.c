/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordbuf.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 11-Aug-2001.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordbuf.h"

#define WORDLEN_DEF 64

int n_wordbuf_open = 0;
int n_wordbuf_close = 0;

struct _wordbuf
{
    int len;
    unsigned char* buf;
    int last;
};

/* function pre-declaration */
static int wordbuf_extend(wordbuf* p, int len);

    wordbuf*
wordbuf_open()
{
    wordbuf *p = (wordbuf*)malloc(sizeof(wordbuf));

    if (p)
    {
	++n_wordbuf_open;
	p->len = WORDLEN_DEF;
	p->buf = (unsigned char*)malloc(p->len);
	p->buf[0] = '\0';
	p->last = 0;
    }
    return p;
}

    void
wordbuf_close(wordbuf* p)
{
    if (p)
    {
	++n_wordbuf_close;
	free(p->buf);
	free(p);
    }
}

    void
wordbuf_reset(wordbuf* p)
{
    p->last = 0;
    p->buf[0] = '\0';
}

/*
 * wordbuf_extend(wordbuf* p, int len);
 *	バッファの伸長。エラー時には0が帰る。
 *	高速化のために伸ばすべきかは呼出側で判断する。
 */
    static int
wordbuf_extend(wordbuf* p, int len)
{
    int newlen = p->len * 2;
    unsigned char *newbuf;

    while (len > newlen)
	newlen *= 2;
    if (!(newbuf = (unsigned char*)realloc(p->buf, newlen)))
    {
	/*fprintf(stderr, "wordbuf_add(): failed to extend buffer\n");*/
	return 0;
    }
    p->len = newlen;
    p->buf = newbuf;

    return len;
}

    int
wordbuf_last(wordbuf* p)
{
    return p->last;
}

    int
wordbuf_add(wordbuf* p, unsigned char ch)
{
    int newlen = p->last + 2;

    if (newlen > p->len && !wordbuf_extend(p, newlen))
	return 0;
    p->buf[p->last++]	= ch;
    p->buf[p->last  ]	= '\0';
    return p->last;
}

    int
wordbuf_cat(wordbuf* p, unsigned char* sz)
{
    int len;

    if (sz && (len = strlen(sz)) > 0)
    {
	int newlen = p->last + len + 1;

	if (newlen > p->len && !wordbuf_extend(p, newlen))
	    return 0;
	memcpy(&p->buf[p->last], sz, len + 1);
	p->last = p->last + len;
    }
    return p->last;
}

    unsigned char*
wordbuf_get(wordbuf* p)
{
    return p->buf;
}
