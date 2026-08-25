// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// stree.h - Static version of mtree

#pragma once

#include <stdint.h>

#include "charset.h"
#include "mtree.h"

#define STREE_INVALID_NODE_ID 0xffffffff

typedef struct stree stree;

typedef struct snode
{
    uint32_t code;
    uint32_t start;
    uint32_t end;
    uint32_t word_id;
} snode;

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
void stree_destroy(stree *st);
stree *stree_from_mtree(mtree *mt);

// Serialization
stree *stree_load(const char *filename);
int stree_save(stree *st, const char *filename);

// Query
uint32_t stree_query(
        stree *st, const unsigned char *query, CHARSET_PROC_CHAR2INT char2int);
const uint8_t * stree_get_words(stree *st, uint32_t node_idx);
void stree_get_children(stree *st, uint32_t node_idx, uint32_t *start, uint32_t *end);

#ifdef __cplusplus
}
#endif
