/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * mnode.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 24-Jan-2002.
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
    wordlist_p list;
};
#define MNODE_MASK_CH		0x000000FF
#define MNODE_GET_CH(p)		((unsigned char)(p)->attr)
#define MNODE_SET_CH(p,c)	((p)->attr = (c))

typedef struct _mtree_t mtree_t, *mtree_p;

/* for mnode_traverse() */
typedef void (*mnode_traverse_proc)(mnode* node, void* data);
#define MNODE_TRAVERSE_PROC mnode_traverse_proc

extern int n_mnode_new;
extern int n_mnode_delete;

C_DECL_BEGIN();
mtree_p mnode_open(FILE* fp);
mtree_p mnode_load(mtree_p root, FILE* fp);
void mnode_close(mtree_p p);
mnode* mnode_query(mtree_p node, unsigned char* query);
void mnode_traverse(mnode *node, MNODE_TRAVERSE_PROC proc, void* data);

/* 主にデバッグ用途 */
void mnode_print(mtree_p mtree, unsigned char* p);
C_DECL_END();

#endif /* MNODE_H */
