/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordbuf.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 21-Jan-2002.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordbuf.h"

#define WORDLEN_DEF 64

int n_wordbuf_open = 0;
int n_wordbuf_close = 0;

/* function pre-declaration */
static int wordbuf_extend(wordbuf_p p, int len);

    wordbuf_p
wordbuf_open()
{
    wordbuf_p p = (wordbuf_p)malloc(sizeof(wordbuf_t));

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
wordbuf_close(wordbuf_p p)
{
    if (p)
    {
	++n_wordbuf_close;
	free(p->buf);
	free(p);
    }
}

    void
wordbuf_reset(wordbuf_p p)
{
    p->last = 0;
    p->buf[0] = '\0';
}

/*
 * wordbuf_extend(wordbuf_p p, int len);
 *	バッファの伸長。エラー時には0が帰る。
 *	高速化のために伸ばすべきかは呼出側で判断する。
 */
    static int
wordbuf_extend(wordbuf_p p, int len)
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
wordbuf_last(wordbuf_p p)
{
    return p->last;
}

    int
wordbuf_add(wordbuf_p p, unsigned char ch)
{
    int newlen = p->last + 2;

    if (newlen > p->len && !wordbuf_extend(p, newlen))
	return 0;
#if 1
    p->buf[p->last++]	= ch;
    p->buf[p->last  ]	= '\0';
#else
    /* リトルエンディアンを仮定するなら使えるが… */
    *(unsigned short*)&p->buf[p->last++] = (unsigned short)ch;
#endif
    return p->last;
}

    int
wordbuf_cat(wordbuf_p p, unsigned char* sz)
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
wordbuf_get(wordbuf_p p)
{
    return p->buf;
}
