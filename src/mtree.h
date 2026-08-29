// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// mtree.h - Migemo tree (mtree) operations
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>
// Need to include <stdio.h>

#pragma once

#include <stdio.h>

#include "charset.h"
#include "wordlist.h"

#define MNODE_BLOCK_BYTES       (64 * 1024)
#define MNODE_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(size_t))
#define MNODE_BLOCK_SIZE                                                       \
    ((MNODE_BLOCK_BYTES - MNODE_BLOCK_HEADER_SIZE) / sizeof(mnode))

typedef struct mnode mnode;
struct mnode
{
    mnode *low, *high;
    mnode *child;

    unsigned int code;
    unsigned int weight;
    wordlist *list;
};

typedef struct mnode_block mnode_block;
struct mnode_block
{
    mnode_block *next;
    size_t used;
    mnode nodes[MNODE_BLOCK_SIZE];
};

typedef struct mnode_arena
{
    mnode_block *head;
    mnode_block *curr;
} mnode_arena;

typedef struct mtree
{
    mnode *rootnode;
    mnode_arena arena;

    CHARSET_PROC_CHAR2INT char2int;
} mtree;

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
mtree *mtree_open(void);
void mtree_close(mtree *mt);

// Load dictionary & query
mtree *mtree_load(mtree *mt, FILE *fp, CHARSET_PROC_CHAR2INT char2int);
mnode *mtree_query(mtree *mt, const unsigned char *query);

// Other operations
mnode *mtree_rootnode(mtree *mt);

// Debug & maintenance
void mtree_print_stat(mtree *mt, const char *title);

#ifdef __cplusplus
}
#endif
