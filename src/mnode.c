// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// mnode.c - mnode interfaces.
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "mnode.h"
#include "wordbuf.h"
#include "wordlist.h"

#define MNODE_DEBUG_STAT 0

#define MNODE_BUFSIZE    16384
#define MNODE_BLOCK_SIZE 1024

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

struct mtree
{
    mnode *rootnode;
    mnode_arena arena;

    CHARSET_PROC_CHAR2INT char2int;
    CHARSET_PROC_INT2CHAR int2char;
};

#if MNODE_DEBUG_STAT

typedef struct
{
    size_t total_nodes;
    size_t low_count;
    size_t high_count;
    size_t child_count;
    size_t word_count;

    int max_sibling_depth;
    size_t total_sibling_depth;

    int max_word_cmp_count;
    size_t total_word_cmp_count;
} mnode_stat;

static void
mnode_debug_stat_stub(
        mnode *node, mnode_stat *stat, int sib_depth, int total_cmp)
{
    if (!node)
        return;

    stat->total_nodes++;
    stat->total_sibling_depth += sib_depth;
    if (sib_depth > stat->max_sibling_depth)
        stat->max_sibling_depth = sib_depth;

    if (node->low)
        stat->low_count++;
    if (node->high)
        stat->high_count++;
    if (node->child)
        stat->child_count++;

    if (node->list)
    {
        stat->word_count++;
        stat->total_word_cmp_count += total_cmp;
        if (total_cmp > stat->max_word_cmp_count)
            stat->max_word_cmp_count = total_cmp;
    }

    // recursive calls for low, high, and child nodes.
    if (node->low)
        mnode_debug_stat_stub(node->low, stat, sib_depth + 1, total_cmp + 1);
    if (node->high)
        mnode_debug_stat_stub(node->high, stat, sib_depth + 1, total_cmp + 1);
    if (node->child)
        mnode_debug_stat_stub(node->child, stat, 0, total_cmp + 1);
}

static void
mnode_debug_stat(mnode *root, mnode_stat *stat)
{
    if (!stat)
        return;
    memset(stat, 0, sizeof(*stat));
    if (!root)
        return;
    mnode_debug_stat_stub(root, stat, 0, 1);
}

static void
mnode_print_stat(const mnode_stat *stat)
{
    if (!stat)
        return;

    printf("=== mnode statistics ===\n");
    printf("Total Nodes          : %zu\n", stat->total_nodes);
    printf("Word Nodes (list)    : %zu\n", stat->word_count);
    printf("Pointer Counts       : low=%zu, high=%zu, child=%zu\n",
            stat->low_count, stat->high_count, stat->child_count);
    printf("Low/High Balance Ratio: %.2f%% / %.2f%%\n",
            stat->low_count + stat->high_count > 0
                    ? (double)stat->low_count
                              / (stat->low_count + stat->high_count) * 100.0
                    : 0.0,
            stat->low_count + stat->high_count > 0
                    ? (double)stat->high_count
                              / (stat->low_count + stat->high_count) * 100.0
                    : 0.0);

    printf("Sibling Depth        : max=%d, avg=%.2f\n", stat->max_sibling_depth,
            stat->total_nodes > 0
                    ? (double)stat->total_sibling_depth / stat->total_nodes
                    : 0.0);

    printf("Word Compare Count   : max=%d, avg=%.2f\n",
            stat->max_word_cmp_count,
            stat->word_count > 0
                    ? (double)stat->total_word_cmp_count / stat->word_count
                    : 0.0);
    printf("======================\n");
}
#endif

static mnode *
mnode_arena_alloc(mnode_arena *arena, unsigned int code)
{
    if (!arena->curr || arena->curr->used >= MNODE_BLOCK_SIZE)
    {
        mnode_block *block = (mnode_block *)calloc(1, sizeof(mnode_block));
        if (!block)
            return NULL;
        if (!arena->head)
            arena->head = block;
        else
            arena->curr->next = block;
        arena->curr = block;
    }
    mnode *p = &arena->curr->nodes[arena->curr->used++];
    p->attr = code;
    return p;
}

static void
mnode_arena_free(mnode_arena *arena)
{
    for (mnode_block *b = arena->head; b;)
    {
        mnode_block *next = b->next;
        for (size_t i = 0; i < b->used; i++)
            if (b->nodes[i].list)
                wordlist_destroy(b->nodes[i].list);
        free(b);
        b = next;
    }
    arena->head = NULL;
    arena->curr = NULL;
}

void
mnode_print_stub(mtree *mt, mnode *vp, unsigned char *p)
{
    static unsigned char buf[256];

    if (!vp)
        return;
    if (!p)
        p = &buf[0];
    int len = mt->int2char(vp->attr, p);
    p[len] = '\0';
    if (vp->list)
        printf("%s (list=%p)\n", buf, vp->list);
    if (vp->child)
        mnode_print_stub(mt, vp->child, p + len);
    if (vp->low)
        mnode_print_stub(mt, vp->low, p);
    if (vp->high)
        mnode_print_stub(mt, vp->high, p);
}

void
mnode_print(mtree *mt, unsigned char *p)
{
    if (mt && mt->rootnode)
        mnode_print_stub(mt, mt->rootnode, p);
}

void
mnode_close(mtree *mt)
{
    if (mt)
    {
        mnode_arena_free(&mt->arena);
        free(mt);
    }
}

// decode_rune decodes a single rune from a string. Returns the number of bytes
// consumed. Always consumes one byte, even for invalid byte sequences.
static inline int
decode_rune(mtree *mt, const unsigned char **pp)
{
    unsigned int code = 0;
    int len = mt->char2int(*pp, &code);
    if (len == 0)
        len = charset_none_char2int(*pp, &code);
    *pp += len;
    return code;
}

static mnode *
search_or_new_mnode(mtree *mt, wordbuf *buf)
{
    // Add to the search tree once the label word is determined
    mnode **res = NULL;

    const unsigned char *word = WORDBUF_GET(buf);
    if (!word)
        return NULL;
    unsigned int code = decode_rune(mt, &word);
    if (code == 0)
        return NULL;

    mnode **ppnext = &mt->rootnode;
    while (1)
    {
        res = ppnext;
        if (!*res)
        {
            *res = mnode_arena_alloc(&mt->arena, code);
            if (!(*res))
                return NULL;
        }
        else
        {
            int pivot = (*res)->attr;
            if (code < pivot)
            {
                ppnext = &(*res)->low;
                continue;
            }
            else if (code > pivot)
            {
                ppnext = &(*res)->high;
                continue;
            }
        }
        ppnext = &(*res)->child;
        code = decode_rune(mt, &word);
        if (code == 0)
            break;
    }

    return *res;
}

// Batch add data from a file to existing nodes.
mtree *
mnode_load(mtree *mt, FILE *fp, CHARSET_PROC_CHAR2INT char2int,
        CHARSET_PROC_INT2CHAR int2char)
{
    mnode *pp = NULL;
    int mode = 0;
    int ch;
    wordbuf *buf;
    wordbuf *prevlabel;
    wordlist **ppword = NULL; // To suppress warning for GCC
    // Variables for the input buffer
    unsigned char cache[MNODE_BUFSIZE];
    unsigned char *cache_ptr = cache;
    unsigned char *cache_tail = cache;

    mt->char2int = char2int;
    mt->int2char = int2char;

    buf = wordbuf_open();
    prevlabel = wordbuf_open();
    if (!fp || !buf || !prevlabel)
    {
        mt = NULL;
        goto END_MNODE_LOAD;
    }

    // EOF handling is ambiguous and doesn't account for malformed files. While
    // it wouldn't be correct without providing a path to EOF from each mode,
    // it's omitted for simplicity. We assume the data file is always
    // well-formed.
    do
    {
        if (cache_ptr >= cache_tail)
        {
            cache_ptr = cache;
            cache_tail = cache + fread(cache, 1, MNODE_BUFSIZE, fp);
            ch = (cache_tail <= cache && feof(fp)) ? EOF : *cache_ptr;
        }
        else
            ch = *cache_ptr;
        ++cache_ptr;

        // Automaton for state 'mode'
        switch (mode)
        {
            case 0: // Label word search mode
                // Whitespace cannot be a label word
                if (isspace(ch) || ch == EOF)
                    continue;
                // Check for comment line
                else if (ch == ';')
                {
                    mode = 2; // Switch to mode that consumes until end of line
                    continue;
                }
                else
                {
                    mode = 1; // Switch to mode for reading label words
                    wordbuf_reset(buf);
                    wordbuf_add(buf, (unsigned char)ch);
                }
                break;

            case 1: // Label word loading mode
                // Detect end of label
                switch (ch)
                {
                    default:
                        wordbuf_add(buf, (unsigned char)ch);
                        break;
                    case '\t':
                        pp = search_or_new_mnode(mt, buf);
                        if (!pp)
                        {
                            mt = NULL;
                            goto END_MNODE_LOAD;
                        }
                        wordbuf_reset(buf);
                        mode = 3; // Switch to mode for skipping whitespace
                                  // before words
                        break;
                }
                break;

            case 2: // Mode that consumes until end of line
                if (ch == '\n')
                {
                    wordbuf_reset(buf);
                    mode = 0; // Return to label word search mode
                }
                break;

            case 3: // Mode for skipping whitespace before words
                if (ch == '\n')
                {
                    wordbuf_reset(buf);
                    mode = 0; // Return to label word search mode
                }
                else if (ch != '\t')
                {
                    // Reset word buffer
                    wordbuf_reset(buf);
                    wordbuf_add(buf, (unsigned char)ch);
                    // Search for the end of the word list (if multiple words
                    // for same label)
                    ppword = &pp->list;
                    while (*ppword)
                        ppword = &(*ppword)->next;
                    mode = 4; // Switch to mode for reading words
                }
                break;

            case 4: // Mode for reading words
                switch (ch)
                {
                    case '\t':
                    case '\n':
                        // Store word
                        *ppword = wordlist_new(
                                WORDBUF_GET(buf), WORDBUF_LEN(buf));
                        wordbuf_reset(buf);

                        if (ch == '\t')
                        {
                            ppword = &(*ppword)->next;
                            mode = 3; // Return to mode for skipping whitespace
                                      // before words
                        }
                        else
                        {
                            ppword = NULL;
                            mode = 0; // Return to label word search mode
                        }
                        break;
                    default:
                        wordbuf_add(buf, (unsigned char)ch);
                        break;
                }
                break;
        }
    }
    while (ch != EOF);

END_MNODE_LOAD:
    wordbuf_close(buf);
    wordbuf_close(prevlabel);
#if MNODE_DEBUG_STAT
    if (mt && mt->rootnode)
    {
        mnode_stat st;
        mnode_debug_stat(mt->rootnode, &st);
        mnode_print_stat(&st);
    }
#endif
    return mt;
}

mtree *
mnode_open(void)
{
    mtree *mt = (mtree *)calloc(1, sizeof(*mt));
    // Set the default to UTF-8.
    mt->char2int = charset_utf8_char2int;
    mt->int2char = charset_utf8_int2char;
    return mt;
}

mnode *
mnode_query(mtree *mt, const unsigned char *query)
{
    if (!mt || !mt->rootnode || !query)
        return NULL;

    unsigned int code = decode_rune(mt, &query);
    if (code == 0)
        return NULL;
    mnode *node = mt->rootnode;

    while (node)
    {
        // Search from siblings
        while (node != NULL && code != node->attr)
            node = code < node->attr ? node->low : node->high;
        if (!node)
            break; // Not found the rune.
        // Proceed to the child node
        code = decode_rune(mt, &query);
        if (code == 0)
            break; // Found the node.
        node = node->child;
    }
    return node;
}

static void
mnode_traverse_stub(mnode *node, MNODE_TRAVERSE_PROC proc, void *data)
{
    if (node->low)
        mnode_traverse_stub(node->low, proc, data);
    proc(node, data);
    if (node->child)
        mnode_traverse_stub(node->child, proc, data);
    if (node->high)
        mnode_traverse_stub(node->high, proc, data);
}

void
mnode_traverse(mnode *node, MNODE_TRAVERSE_PROC proc, void *data)
{
    if (node && proc)
    {
        proc(node, data);
        if (node->child)
            mnode_traverse_stub(node->child, proc, data);
    }
}
