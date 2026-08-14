// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// mtree.h - Migemo tree (mtree) operations
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>
// Need to include <stdio.h>

#pragma once

#include "wordlist.h"

typedef struct mtree mtree;

// Tree object
typedef struct mnode mnode;
struct mnode
{
    mnode *low, *high;
    mnode *child;

    unsigned int code;
    unsigned int weight;
    wordlist *list;
};

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
mtree *mtree_open(void);
void mtree_close(mtree *mt);

// Load dictionary & query
mtree *mtree_load(mtree *mt, FILE *fp, CHARSET_PROC_CHAR2INT char2int);
mnode *mtree_query(mtree *mt, const unsigned char *query);

// Debug & maintenance
void mtree_print_stat(mtree *mt, const char *title);

#ifdef __cplusplus
}
#endif
