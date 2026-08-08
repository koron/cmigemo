// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// rxgen.c - regular expression generator
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rxgen.h"
#include "wordbuf.h"

#if defined(_MSC_VER)
# define STRDUP _strdup
#else
# define STRDUP strdup
#endif

#define RXGEN_ENC_SJISTINY
// #define RXGEN_OP_VIM

#define RXGEN_OP_MAXLEN     8
#define RXGEN_OP_OR         "|"
#define RXGEN_OP_NEST_IN    "("
#define RXGEN_OP_NEST_OUT   ")"
#define RXGEN_OP_SELECT_IN  "["
#define RXGEN_OP_SELECT_OUT "]"
#define RXGEN_OP_NEWLINE    ""

#define RXGEN_DEBUG_STAT 0

#define RNODE_BLOCK_SIZE 1024

typedef struct _rnode rnode;
struct _rnode
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

struct _rxgen
{
    rnode *root;
    rnode_arena arena;

    RXGEN_PROC_CHAR2INT char2int;
    RXGEN_PROC_INT2CHAR int2char;

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
default_char2int(const unsigned char *in, unsigned int *out)
{
    if (out)
        *out = *in;
    return 1;
}

static int
default_int2char(unsigned int in, unsigned char *out)
{
    int len = 0;
    // Assume that out has at least 16 bytes
    switch (in)
    {
        case '\\':
        case '.':
        case '*':
        case '^':
        case '$':
        case '/':
#ifdef RXGEN_OP_VIM
        case '[':
        case ']':
        case '~':
#endif
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
rxgen_setproc_char2int(rxgen *object, RXGEN_PROC_CHAR2INT proc)
{
    if (object)
        object->char2int = proc ? proc : default_char2int;
}

void
rxgen_setproc_int2char(rxgen *object, RXGEN_PROC_INT2CHAR proc)
{
    if (object)
        object->int2char = proc ? proc : default_int2char;
}

static inline int
rxgen_call_char2int(rxgen *object, const unsigned char *pch, unsigned int *code)
{
    int len = object->char2int(pch, code);
    return len ? len : default_char2int(pch, code);
}

static int
rxgen_call_int2char(rxgen *object, unsigned int code, unsigned char *buf)
{
    int len = object->int2char(code, buf);
    return len ? len : default_int2char(code, buf);
}

rxgen *
rxgen_open()
{
    rxgen *object = (rxgen *)calloc(1, sizeof(rxgen));
    if (object)
    {
        rxgen_setproc_char2int(object, NULL);
        rxgen_setproc_int2char(object, NULL);
        strcpy(object->op_or, RXGEN_OP_OR);
        strcpy(object->op_nest_in, RXGEN_OP_NEST_IN);
        strcpy(object->op_nest_out, RXGEN_OP_NEST_OUT);
        strcpy(object->op_select_in, RXGEN_OP_SELECT_IN);
        strcpy(object->op_select_out, RXGEN_OP_SELECT_OUT);
        strcpy(object->op_newline, RXGEN_OP_NEWLINE);
    }
    return object;
}

void
rxgen_close(rxgen *object)
{
    if (object)
    {
        rnode_arena_free(&object->arena);
        free(object);
    }
}

int
rxgen_add(rxgen *object, const unsigned char *word)
{
    if (!word)
        return 0;

    rnode **ppnode = &object->root;
    rnode *pnode = NULL;
    while (1)
    {
        unsigned int code;
        int len = rxgen_call_char2int(object, word, &code);

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

        pnode = rnode_dig(&object->arena, ppnode, code);
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

static void rxgen_generate_stub(rxgen *object, wordbuf *buf, rnode *node);

static void
rxgen_write_node_code(rxgen *object, wordbuf *buf, rnode *node)
{
    unsigned char bytes[6];
    int len = rxgen_call_int2char(object, node->code, bytes);
    wordbuf_write_bytes(buf, bytes, len);
}

static void
rxgen_write_node_no_children(rxgen *object, wordbuf *buf, rnode *node)
{
    if (node->low)
        rxgen_write_node_no_children(object, buf, node->low);
    if (node->child == NULL)
        rxgen_write_node_code(object, buf, node);
    if (node->high)
        rxgen_write_node_no_children(object, buf, node->high);
}

static void
rxgen_write_node_has_children(
        rxgen *object, wordbuf *buf, rnode *node, bool *needOr)
{
    if (node->low)
        rxgen_write_node_has_children(object, buf, node->low, needOr);
    if (node->child != NULL)
    {
        // Output OR if necessary
        if (*needOr)
            wordbuf_cat(buf, object->op_or);
        rxgen_write_node_code(object, buf, node);
        // Insert a pattern that skips whitespace/newline
        if (object->op_newline[0])
            wordbuf_cat(buf, object->op_newline);
        rxgen_generate_stub(object, buf, node->child);
        *needOr = true;
    }
    if (node->high)
        rxgen_write_node_has_children(object, buf, node->high, needOr);
}

static void
rxgen_generate_stub(rxgen *object, wordbuf *buf, rnode *node)
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
        wordbuf_cat(buf, object->op_nest_in);

    // Group nodes without children first with []
    if (noChildrenCount > 0)
    {
        if (needClass)
        {
            wordbuf_cat(buf, object->op_select_in);
            rxgen_write_node_no_children(object, buf, node);
            wordbuf_cat(buf, object->op_select_out);
        }
        else
            rxgen_write_node_no_children(object, buf, node);
    }

    // Output nodes with children
    if (childrenCount > 0)
    {
        bool needOr = noChildrenCount > 0;
        rxgen_write_node_has_children(object, buf, node, &needOr);
    }

    // Group using () if necessary
    if (needGroup)
        wordbuf_cat(buf, object->op_nest_out);
}

#if RXGEN_DEBUG_STAT

typedef struct
{
    size_t total_nodes;
    size_t low_count;
    size_t high_count;
    size_t child_count;
    size_t wordtail_count;

    int max_sibling_depth;
    size_t total_sibling_depth;

    int max_node_cmp_count;
    size_t total_node_cmp_count;
} rnode_stat;

static void
rnode_debug_stat_stub(
        rnode *node, rnode_stat *stat, int sib_depth, int total_cmp)
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
    rnode_stat stat;
    memset(&stat, 0, sizeof(stat));
    if (!root)
        return;

    rnode_debug_stat_stub(root, &stat, 0, 1);

    printf("=== rnode statistics ===\n");
    printf("Total Nodes          : %zu\n", stat.total_nodes);
    printf("Wordtail Nodes       : %zu\n", stat.wordtail_count);
    printf("Pointer Counts       : low=%zu, high=%zu, child=%zu\n",
            stat.low_count, stat.high_count, stat.child_count);
    printf("Low/High Balance Ratio: %.2f%% / %.2f%%\n",
            stat.low_count + stat.high_count > 0
                    ? (double)stat.low_count
                              / (stat.low_count + stat.high_count) * 100.0
                    : 0.0,
            stat.low_count + stat.high_count > 0
                    ? (double)stat.high_count
                              / (stat.low_count + stat.high_count) * 100.0
                    : 0.0);
    printf("Sibling Depth        : max=%d, avg=%.2f\n", stat.max_sibling_depth,
            stat.total_nodes > 0
                    ? (double)stat.total_sibling_depth / stat.total_nodes
                    : 0.0);
    printf("Node Compare Count   : max=%d, avg=%.2f\n", stat.max_node_cmp_count,
            stat.total_nodes > 0
                    ? (double)stat.total_node_cmp_count / stat.total_nodes
                    : 0.0);
    printf("======================\n");
}

#endif

unsigned char *
rxgen_generate(rxgen *object)
{
    unsigned char *answer = NULL;
    wordbuf *buf;

    if (object && (buf = wordbuf_open()))
    {

        if (object->root)
        {
#if RXGEN_DEBUG_STAT
            rnode_debug_stat(object->root);
#endif
            rxgen_generate_stub(object, buf, object->root);
        }
        answer = STRDUP(WORDBUF_GET(buf));
        wordbuf_close(buf);
    }
    return answer;
}

void
rxgen_release(rxgen *object, unsigned char *string)
{
    free(string);
}

// Reset all patterns added via rxgen_add()
void
rxgen_reset(rxgen *object)
{
    if (object)
    {
        rnode_arena_free(&object->arena);
        object->root = NULL;
    }
}

static unsigned char *
rxgen_get_operator_stub(rxgen *object, int index)
{
    switch (index)
    {
        case RXGEN_OPINDEX_OR:
            return object->op_or;
        case RXGEN_OPINDEX_NEST_IN:
            return object->op_nest_in;
        case RXGEN_OPINDEX_NEST_OUT:
            return object->op_nest_out;
        case RXGEN_OPINDEX_SELECT_IN:
            return object->op_select_in;
        case RXGEN_OPINDEX_SELECT_OUT:
            return object->op_select_out;
        case RXGEN_OPINDEX_NEWLINE:
            return object->op_newline;
        default:
            return NULL;
    }
}

const unsigned char *
rxgen_get_operator(rxgen *object, int index)
{
    return (const unsigned char *)(object ? rxgen_get_operator_stub(
                                                    object, index)
                                          : NULL);
}

int
rxgen_set_operator(rxgen *object, int index, const unsigned char *op)
{
    unsigned char *dest;

    if (!object)
        return 1; // Invalid object
    if (strlen(op) >= RXGEN_OP_MAXLEN)
        return 2; // Too long operator
    if (!(dest = rxgen_get_operator_stub(object, index)))
        return 3; // No such an operator
    strcpy(dest, op);

    return 0;
}
