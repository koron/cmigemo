/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * rxgen.h - regular expression generator
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 05-Aug-2001.
 */

#ifndef RXGEN_H
#define RXGEN_H

#include "cdecl.h"

typedef struct _rxgen rxgen;
typedef int (*rxgen_proc_char2int)(unsigned char*, unsigned int*);
typedef int (*rxgen_proc_int2char)(unsigned int, unsigned char*);
#define RXGEN_PROC_CHAR2INT rxgen_proc_char2int
#define RXGEN_PROC_INT2CHAR rxgen_proc_int2char

extern int n_rnode_new;
extern int n_rnode_delete;

C_DECL_BEGIN();
rxgen* rxgen_open();
void rxgen_close(rxgen* object);
int rxgen_add(rxgen* object, unsigned char* word);
unsigned char* rxgen_generate(rxgen* object);
void rxgen_release(rxgen* object, unsigned char* string);

void rxgen_setproc_char2int(rxgen* object, RXGEN_PROC_CHAR2INT proc);
void rxgen_setproc_int2char(rxgen* object, RXGEN_PROC_INT2CHAR proc);
C_DECL_END();

#endif /* RXGEN_H */
