// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// rxgen.c - regular expression generator
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <stdbool.h>
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

static rnode *
rnode_dig(rnode_arena *arena, rnode **pp, unsigned int code)
{
    rnode *p = *pp;
    if (p == NULL)
    {
        *pp = rnode_arena_alloc(arena, code);
        return *pp;
    }
    while (1)
    {
        if (code == p->code)
            return p;
        if (code < p->code)
        {
            if (p->low == NULL)
            {
                p->low = rnode_arena_alloc(arena, code);
                return p->low;
            }
            p = p->low;
        }
        else
        {
            if (p->high == NULL)
            {
                p->high = rnode_arena_alloc(arena, code);
                return p->high;
            }
            p = p->high;
        }
    }
}

// rxgen interfaces

static int
rxgen_char2int_fallback(const unsigned char *in, unsigned int *out)
{
    if (out)
        *out = *in;
    return 1;
}

static int
rxgen_int2char_fallback(unsigned int in, unsigned char *out)
{
    int len = 0;
    // Assume that out has at least 16 bytes
    switch (in)
    {
        case '\\':
        case '.':
        case '*':
        case '+':
        case '^':
        case '$':
        case '/':
            if (out)
                out[len] = '\\';
            ++len;
        default:
            if (out)
                out[len] = (unsigned char)(in & 0xFF);
            ++len;
            break;
    }
    return len;
}

void
rxgen_setproc_char2int(rxgen *rx, CHARSET_PROC_CHAR2INT proc)
{
    if (rx)
        rx->char2int = proc ? proc : rxgen_char2int_fallback;
}

void
rxgen_setproc_int2char(rxgen *rx, CHARSET_PROC_INT2CHAR proc)
{
    if (rx)
        rx->int2char = proc ? proc : rxgen_int2char_fallback;
}

static inline int
rxgen_call_char2int(rxgen *rx, const unsigned char *pch, unsigned int *code)
{
    int len = rx->char2int(pch, code);
    return len ? len : rxgen_char2int_fallback(pch, code);
}

static int
rxgen_call_int2char(rxgen *rx, unsigned int code, unsigned char *buf)
{
    int len = rx->int2char(code, buf);
    return len ? len : rxgen_int2char_fallback(code, buf);
}

rxgen *
rxgen_open()
{
    rxgen *rx = (rxgen *)calloc(1, sizeof(rxgen));
    if (rx)
    {
        rxgen_setproc_char2int(rx, NULL);
        rxgen_setproc_int2char(rx, NULL);
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

int
rxgen_add(rxgen *rx, const unsigned char *word)
{
    if (!word)
        return 0;

    rnode **ppnode = &rx->root;
    rnode *pnode = NULL;
    while (1)
    {
        unsigned int code;
        int len = rxgen_call_char2int(rx, word, &code);

        // Terminate if the input pattern is exhausted
        if (code == 0)
        {
            if (pnode)
                pnode->wordtail = true;
            if (*ppnode)
            {
                // If a longer word is already registered than the one being
                // registered, discard the longer one. E.g.:
                //      赤ちゃん + 赤   -> 赤
                //      国際便   + 国際 -> 国際
                *ppnode = NULL;
                // FIXME: mark *ppnode here for future use when collecting
                // statistical information or reusing reclaimed nodes.
            }
            break;
        }

        word += len;

        pnode = rnode_dig(&rx->arena, ppnode, code);
        if (!pnode)
            return 0; // allocation error.
        ppnode = &pnode->child;

        if (pnode && pnode->wordtail)
        {
            // If a shorter word is already registered than the one being
            // registered, discard the remaining characters. E.g.:
            //      赤ちゃん + 赤   -> 赤
            //      国際便   + 国際 -> 国際
            return 2; // not registered a word, but found short one.
        }
        // Move the focus deeper by traversing child nodes
    }
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

static void rxgen_generate_stub(rxgen *rx, strbuf *buf, rnode *node);

static void
rxgen_write_node_code(rxgen *rx, strbuf *buf, rnode *node)
{
    unsigned char bytes[6];
    int len = rxgen_call_int2char(rx, node->code, bytes);
    strbuf_append_mem(buf, bytes, len);
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
