/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * mnode.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 04-Aug-2001.
 */
/*
 * Need to include <stdio.h>
 */

#ifndef MNODE_H
#define MNODE_H

#include "cdecl.h"

typedef struct _wordlist wordlist;

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
#define MNODE_MASK_TERM		0x80000000
#define MNODE_GET_CH(p)		((unsigned char)((p)->attr & MNODE_MASK_CH))
#define MNODE_SET_CH(p,c)	((p)->attr = ((p)->attr & ~MNODE_MASK_CH) | ((c) & MNODE_MASK_CH))
#define MNODE_GET_TERM(p)	((p)->attr & MNODE_MASK_TERM ? 1 : 0)
#define MNODE_SET_TERM(p,t)	((p)->attr = ((p)->attr & ~MNODE_MASK_TERM) | (t ? MNODE_MASK_TERM : 0))

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
C_DECL_END();

#endif /* MNODE_H */
