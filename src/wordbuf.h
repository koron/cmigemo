/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordbuf.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 07-Aug-2001.
 */
#ifndef WORDBUF_H
#define WORDBUF_H

#include "cdecl.h"

typedef struct _wordbuf wordbuf;

extern int n_wordbuf_open;
extern int n_wordbuf_close;

C_DECL_BEGIN();
wordbuf* wordbuf_open();
void wordbuf_close(wordbuf* p);
void wordbuf_reset(wordbuf* p);
int wordbuf_last(wordbuf* p);
int wordbuf_add(wordbuf* p, unsigned char ch);
int wordbuf_cat(wordbuf* p, unsigned char* sz);
unsigned char* wordbuf_get(wordbuf* p);
C_DECL_END();

#endif /* WORDBUF_H */
