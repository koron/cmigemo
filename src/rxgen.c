// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// rxgen.c - regular expression generator
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rxgen.h"
#include "strbuf.h"
#include "trie.h"

#if defined(_MSC_VER)
# define STRDUP _strdup
#else
# define STRDUP strdup
#endif

#define RXGEN_OP_MAXLEN     8
#define RXGEN_OP_OR         "|"
#define RXGEN_OP_NEST_IN    "("
#define RXGEN_OP_NEST_OUT   ")"
#define RXGEN_OP_SELECT_IN  "["
#define RXGEN_OP_SELECT_OUT "]"
#define RXGEN_OP_NEWLINE    ""

#define RXGEN_DEBUG_STAT 0

#define RNODE_BLOCK_SIZE 1024

typedef struct rnode rnode;
struct rnode
{
    rnode *low, *high;
    rnode *child;

    uint8_t height;

    unsigned int code;
    bool wordtail;
};

typedef struct rnode_block rnode_block;
struct rnode_block
{
    rnode_block *next;
    size_t used;
    rnode nodes[RNODE_BLOCK_SIZE];
};

typedef struct rnode_arena
{
    rnode_block *head;
    rnode_block *curr;
} rnode_arena;

struct rxgen
{
    rnode *root;
    rnode_arena arena;

    CHARSET_PROC_CHAR2INT char2int;
    CHARSET_PROC_INT2CHAR int2char;

    uint32_t escapes_bitmap[3]; // 3 = (126 - 32 + 1 + 31) / 32

    unsigned char op_or[RXGEN_OP_MAXLEN];
    unsigned char op_nest_in[RXGEN_OP_MAXLEN];
    unsigned char op_nest_out[RXGEN_OP_MAXLEN];
    unsigned char op_select_in[RXGEN_OP_MAXLEN];
    unsigned char op_select_out[RXGEN_OP_MAXLEN];
    unsigned char op_newline[RXGEN_OP_MAXLEN];
};

// rnode interfaces

static rnode *
rnode_arena_alloc(rnode_arena *arena, unsigned code)
{
    if (!arena->curr || arena->curr->used >= RNODE_BLOCK_SIZE)
    {
        rnode_block *block = (rnode_block *)calloc(1, sizeof(rnode_block));
        if (!block)
            return NULL;
        if (!arena->head)
            arena->head = block;
        else
            arena->curr->next = block;
        arena->curr = block;
    }
    rnode *p = &arena->curr->nodes[arena->curr->used++];
    p->height = 1;
    p->code = code;
    return p;
}

static void
rnode_arena_free(rnode_arena *arena)
{
    if (arena == NULL)
        return;
    for (rnode_block *p = arena->head; p;)
    {
        rnode_block *tmp = p;
        p = p->next;
        free(tmp);
    }
    arena->head = NULL;
    arena->curr = NULL;
}

// rxgen interfaces

void
rxgen_setproc_char2int(rxgen *rx, CHARSET_PROC_CHAR2INT proc)
{
    if (rx)
        rx->char2int = proc ? proc : charset_none_char2int;
}

void
rxgen_setproc_int2char(rxgen *rx, CHARSET_PROC_INT2CHAR proc)
{
    if (rx)
        rx->int2char = proc ? proc : charset_none_int2char;
}

void
rxgen_set_escape_chars(rxgen *rx, const unsigned char *chars)
{
    if (!rx)
        return;

    memset(rx->escapes_bitmap, 0, sizeof(rx->escapes_bitmap));

    if (chars == NULL)
        chars = (const unsigned char *)"\\.*+^$/";

    for (const unsigned char *p = chars; *p; ++p)
    {
        unsigned char c = *p;
        if (c >= 32 && c <= 126)
        {
            unsigned int idx = c - 32;
            rx->escapes_bitmap[idx / 32] |= (1U << (idx % 32));
        }
    }
}

rxgen *
rxgen_open()
{
    rxgen *rx = (rxgen *)calloc(1, sizeof(rxgen));
    if (rx)
    {
        rxgen_setproc_char2int(rx, NULL);
        rxgen_setproc_int2char(rx, NULL);
        rxgen_set_escape_chars(rx, NULL);
        strcpy(rx->op_or, RXGEN_OP_OR);
        strcpy(rx->op_nest_in, RXGEN_OP_NEST_IN);
        strcpy(rx->op_nest_out, RXGEN_OP_NEST_OUT);
        strcpy(rx->op_select_in, RXGEN_OP_SELECT_IN);
        strcpy(rx->op_select_out, RXGEN_OP_SELECT_OUT);
        strcpy(rx->op_newline, RXGEN_OP_NEWLINE);
    }
    return rx;
}

void
rxgen_close(rxgen *rx)
{
    if (rx)
    {
        rnode_arena_free(&rx->arena);
        free(rx);
    }
}

static inline uint8_t
rnode_height(rnode *node)
{
    return node ? node->height : 0;
}

static inline void
rnode_update_height(rnode *p)
{
    uint8_t l = rnode_height(p->low);
    uint8_t h = rnode_height(p->high);
    p->height = 1 + (l > h ? l : h);
}

static inline rnode *
rnode_rotate_left(rnode *node)
{
    rnode *r = node->high;
    node->high = r->low;
    r->low = node;
    rnode_update_height(node);
    rnode_update_height(r);
    return r;
}

static inline rnode *
rnode_rotate_right(rnode *node)
{
    rnode *l = node->low;
    node->low = l->high;
    l->high = node;
    rnode_update_height(node);
    rnode_update_height(l);
    return l;
}

static inline rnode *
rnode_balance(rnode_arena *arena, rnode *node)
{
    if (!node)
        return NULL;
    uint8_t l = rnode_height(node->low);
    uint8_t r = rnode_height(node->high);

    if (r > l + 1)
    {
        if (rnode_height(node->high->high) < rnode_height(node->high->low))
            node->high = rnode_rotate_right(node->high);
        return rnode_rotate_left(node);
    }
    else if (l > r + 1)
    {
        if (rnode_height(node->low->low) < rnode_height(node->low->high))
            node->low = rnode_rotate_left(node->low);
        return rnode_rotate_right(node);
    }

    rnode_update_height(node);
    return node;
}

static inline rnode *
rnode_dig2balance(rnode_arena *arena, rnode **pp, unsigned int code)
{
    rnode **path[32];
    int path_top = 0;

    while (*pp) {
        path[path_top++] = pp;
        unsigned int pivot = (*pp)->code;
        pp = (code > pivot) ? &(*pp)->high : &(*pp)->low;
    }

    *pp = rnode_arena_alloc(arena, code);
    rnode *ret = *pp;

    while (path_top > 0) {
        rnode **p = path[--path_top];
        *p = rnode_balance(arena, *p);
    }

    return ret;
}

static inline rnode *
rnode_dig2(rnode_arena *arena, rnode **pp, unsigned int code, bool balance)
{
    rnode **root = pp;
    while (*pp)
    {
        unsigned int pivot = (*pp)->code;
        if (code > pivot)
            pp = &(*pp)->high;
        else if (code < pivot)
            pp = &(*pp)->low;
        else
            return *pp;
    }
    if (!balance)
    {
        *pp = rnode_arena_alloc(arena, code);
        return *pp;
    }
    return rnode_dig2balance(arena, root, code);
}

int
rxgen_add(rxgen *rx, const unsigned char *word)
{
    if (!word)
        return 0;

    rnode **ppnode = &rx->root;
    rnode *pnode = NULL;

    bool balance = true;
    while (1)
    {
        unsigned int code = charset_decode(rx->char2int, &word);

        // Terminate if the input pattern is exhausted
        if (code == 0)
            break;

        pnode = rnode_dig2(&rx->arena, ppnode, code, balance);
        if (!pnode)
            return 0; // allocation error.
        if (pnode->wordtail)
            // If a shorter word is already registered than the one being
            // registered, discard the remaining characters. E.g.:
            //      赤   + 赤ちゃん -> 赤
            //      国際 + 国際便   -> 国際
            return 2; // not registered a word, but found short one.

        // Move the focus deeper by traversing child nodes
        ppnode = &pnode->child;
        balance = false;
    }

    if (pnode)
        pnode->wordtail = true;
    if (*ppnode)
        // If a longer word is already registered than the one being
        // registered, discard the longer one. E.g.:
        //      赤ちゃん + 赤   -> 赤
        //      国際便   + 国際 -> 国際
        *ppnode = NULL;
    return 1; // registered a word, some nodes.
}

static void
rxgen_rnode_count(rnode *node, int *childrenCount, int *brotherCount)
{
    if (node->child)
        (*childrenCount)++;
    if (node->low)
    {
        (*brotherCount)++;
        rxgen_rnode_count(node->low, childrenCount, brotherCount);
    }
    if (node->high)
    {
        (*brotherCount)++;
        rxgen_rnode_count(node->high, childrenCount, brotherCount);
    }
}

static inline void
rxgen_append_ch(rxgen *rx, strbuf *buf, unsigned int code)
{
    unsigned char bytes[CHARSET_MAX_BYTES];
    int len = rx->int2char(code, bytes);
    strbuf_append_mem(buf, bytes, len);
}

static inline bool
rxgen_is_escape(rxgen *rx, unsigned int code)
{
    if (code < 32 || code > 126)
        return false;
    unsigned int idx = code - 32;
    return rx->escapes_bitmap[idx / 32] & (1U << (idx % 32));
}

static void
rxgen_write_node_code(rxgen *rx, strbuf *buf, rnode *node)
{
    unsigned int code = node->code;
    if (rxgen_is_escape(rx, code))
        rxgen_append_ch(rx, buf, '\\');
    rxgen_append_ch(rx, buf, code);
}

static void
rxgen_write_node_no_children(rxgen *rx, strbuf *buf, rnode *node)
{
    if (node->low)
        rxgen_write_node_no_children(rx, buf, node->low);
    if (node->child == NULL)
        rxgen_write_node_code(rx, buf, node);
    if (node->high)
        rxgen_write_node_no_children(rx, buf, node->high);
}

static void rxgen_generate_stub(rxgen *rx, strbuf *buf, rnode *node);

static void
rxgen_write_node_has_children(rxgen *rx, strbuf *buf, rnode *node, bool *needOr)
{
    if (node->low)
        rxgen_write_node_has_children(rx, buf, node->low, needOr);
    if (node->child != NULL)
    {
        // Output OR if necessary
        if (*needOr)
            strbuf_append_str(buf, rx->op_or);
        rxgen_write_node_code(rx, buf, node);
        // Insert a pattern that skips whitespace/newline
        if (rx->op_newline[0])
            strbuf_append_str(buf, rx->op_newline);
        rxgen_generate_stub(rx, buf, node->child);
        *needOr = true;
    }
    if (node->high)
        rxgen_write_node_has_children(rx, buf, node->high, needOr);
}

static void
rxgen_generate_stub(rxgen *rx, strbuf *buf, rnode *node)
{
    // Check characteristics of the current level (number of siblings, number of
    // children)
    int childrenCount = 0;
    int brotherCount = 1;
    rxgen_rnode_count(node, &childrenCount, &brotherCount);
    int noChildrenCount = brotherCount - childrenCount;
#if 0 // For debug
    printf("node=%p code=%04X\n  noChildrenCount=%d childrenCount=%d brotherCount=%d\n",
	    node, node->code, noChildrenCount, childrenCount, brotherCount);
#endif

    bool needGroup = brotherCount > 1 && childrenCount > 0;
    bool needClass = noChildrenCount > 1;

    // Group using () if necessary
    if (needGroup)
        strbuf_append_str(buf, rx->op_nest_in);

    // Group nodes without children first with []
    if (noChildrenCount > 0)
    {
        if (needClass)
        {
            strbuf_append_str(buf, rx->op_select_in);
            rxgen_write_node_no_children(rx, buf, node);
            strbuf_append_str(buf, rx->op_select_out);
        }
        else
            rxgen_write_node_no_children(rx, buf, node);
    }

    // Output nodes with children
    if (childrenCount > 0)
    {
        bool needOr = noChildrenCount > 0;
        rxgen_write_node_has_children(rx, buf, node, &needOr);
    }

    // Group using () if necessary
    if (needGroup)
        strbuf_append_str(buf, rx->op_nest_out);
}

#if RXGEN_DEBUG_STAT

static void
rnode_debug_stat_stub(
        rnode *node, trie_stat *stat, int sib_depth, int total_cmp)
{
    if (!node)
        return;

    stat->total_nodes++;
    stat->total_sibling_depth += sib_depth;
    if (sib_depth > stat->max_sibling_depth)
        stat->max_sibling_depth = sib_depth;

    stat->total_node_cmp_count += total_cmp;
    if (total_cmp > stat->max_node_cmp_count)
        stat->max_node_cmp_count = total_cmp;

    if (node->low)
        stat->low_count++;
    if (node->high)
        stat->high_count++;
    if (node->child)
        stat->child_count++;
    if (node->wordtail)
        stat->wordtail_count++;

    if (node->low)
        rnode_debug_stat_stub(node->low, stat, sib_depth + 1, total_cmp + 1);
    if (node->high)
        rnode_debug_stat_stub(node->high, stat, sib_depth + 1, total_cmp + 1);
    if (node->child)
        rnode_debug_stat_stub(node->child, stat, 0, total_cmp + 1);
}

static void
rnode_debug_stat(rnode *root)
{
    trie_stat stat;
    memset(&stat, 0, sizeof(stat));
    if (!root)
        return;

    rnode_debug_stat_stub(root, &stat, 0, 1);
    trie_stat_print(&stat, "rnode statistics");
}

#endif

unsigned char *
rxgen_generate(rxgen *rx)
{
    unsigned char *answer = NULL;
    strbuf *buf;

    if (rx && (buf = strbuf_open()))
    {

        if (rx->root)
        {
#if RXGEN_DEBUG_STAT
            rnode_debug_stat(rx->root);
#endif
            rxgen_generate_stub(rx, buf, rx->root);
        }
        answer = STRDUP(strbuf_get(buf));
        strbuf_close(buf);
    }
    return answer;
}

void
rxgen_release(rxgen *rx, unsigned char *s)
{
    free(s);
}

// Reset all patterns added via rxgen_add()
void
rxgen_reset(rxgen *rx)
{
    if (rx)
    {
        rnode_arena_free(&rx->arena);
        rx->root = NULL;
    }
}

static unsigned char *
rxgen_get_operator_stub(rxgen *rx, int index)
{
    switch (index)
    {
        case RXGEN_OPINDEX_OR:
            return rx->op_or;
        case RXGEN_OPINDEX_NEST_IN:
            return rx->op_nest_in;
        case RXGEN_OPINDEX_NEST_OUT:
            return rx->op_nest_out;
        case RXGEN_OPINDEX_SELECT_IN:
            return rx->op_select_in;
        case RXGEN_OPINDEX_SELECT_OUT:
            return rx->op_select_out;
        case RXGEN_OPINDEX_NEWLINE:
            return rx->op_newline;
        default:
            return NULL;
    }
}

const unsigned char *
rxgen_get_operator(rxgen *rx, int index)
{
    return (const unsigned char *)(rx ? rxgen_get_operator_stub(rx, index)
                                      : NULL);
}

int
rxgen_set_operator(rxgen *rx, int index, const unsigned char *op)
{
    unsigned char *dest;

    if (!rx)
        return 1; // Invalid rx
    if (strlen(op) >= RXGEN_OP_MAXLEN)
        return 2; // Too long operator
    if (!(dest = rxgen_get_operator_stub(rx, index)))
        return 3; // No such an operator
    strcpy(dest, op);

    return 0;
}
