/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * mnode.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 09-Aug-2001.
 */
/*
 * Need to include <stdio.h>
 */

#ifndef MNODE_H
#define MNODE_H

#include "cdecl.h"

/* ツリーオブジェクト */
typedef struct _mnode mnode;
struct _mnode
{
    unsigned int attr;
    mnode* next;
    mnode* child;
    wordlist *list;
};
#define MNODE_MASK_CH		0x000000FF
#define MNODE_GET_CH(p)		((unsigned char)(p)->attr)
#define MNODE_SET_CH(p,c)	((p)->attr = (c))

/* for mnode_traverse() */
typedef void (*mnode_traverse_proc)(mnode* node, void* data);
#define MNODE_TRAVERSE_PROC mnode_traverse_proc

extern int n_mnode_new;
extern int n_mnode_delete;

C_DECL_BEGIN();
mnode* mnode_open(FILE* fp);
void mnode_close(mnode* p);
mnode* mnode_query(mnode* node, unsigned char* query);
void mnode_traverse(mnode* node, MNODE_TRAVERSE_PROC proc, void* data);

/* 主にデバッグ用途 */
void mnode_print(mnode* vp, unsigned char* p);
C_DECL_END();

#endif /* MNODE_H */
