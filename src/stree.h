// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// stree.h - Static version of mtree

#pragma once

#include <stdint.h>

#include "charset.h"
#include "mtree.h"

#define STREE_HEAD_ID_V1      "MGS1"
#define STREE_INVALID_WORD_ID 0xffffffff
#define STREE_INVALID_NODE_ID 0xffffffff

typedef struct stree stree;

typedef struct snode
{
    uint32_t code;
    uint32_t start;
    uint32_t end;
    uint32_t word_id;
} snode;

typedef struct stree_header
{
    uint8_t id[4];
    uint32_t node_count;
    uint32_t word_count;
    uint32_t word_buf_size;
} stree_header;

typedef struct stree
{
    stree_header head;

    snode *nodes;
    uint32_t *word_off;
    uint8_t *word_buf;
} stree;

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

#ifdef __cplusplus
}
#endif

static inline const uint8_t *
stree_get_words(stree *st, uint32_t node_idx)
{
    if (node_idx >= st->head.node_count)
        return NULL;
    uint32_t word_id = st->nodes[node_idx].word_id;
    if (word_id >= st->head.word_count)
        return NULL;
    return &st->word_buf[st->word_off[word_id]];
}

static inline void
stree_get_children(stree *st, uint32_t node_idx, uint32_t *start, uint32_t *end)
{
    if (node_idx >= st->head.node_count)
        return;
    *start = st->nodes[node_idx].start;
    *end = st->nodes[node_idx].end;
}
