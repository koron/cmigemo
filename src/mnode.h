// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// mnode.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>
// Need to include <stdio.h>

#pragma once

#include "wordlist.h"

// Tree object
typedef struct mnode mnode;
struct mnode
{
    unsigned int attr;
    mnode *low, *high;
    mnode *child;
    wordlist *list;
};

typedef struct mtree mtree;

// for mnode_traverse()
typedef void (*mnode_traverse_proc)(mnode *node, void *data);
#define MNODE_TRAVERSE_PROC mnode_traverse_proc

#ifdef __cplusplus
extern "C" {
#endif

mtree *mnode_open(void);

mtree *mnode_load(mtree *root, FILE *fp, CHARSET_PROC_CHAR2INT char2int,
        CHARSET_PROC_INT2CHAR int2char);

void mnode_close(mtree *p);

mnode *mnode_query(mtree *node, const unsigned char *query);

void mnode_traverse(mnode *node, MNODE_TRAVERSE_PROC proc, void *data);

// Mainly for debugging purposes
void mnode_print(mtree *mt, unsigned char *p);

void mnode_print_stat(mtree *mt);

#ifdef __cplusplus
}
#endif
