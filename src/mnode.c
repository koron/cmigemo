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

#include "mnode.h"
#include "wordbuf.h"
#include "wordlist.h"

#define MNODE_DEBUG_STAT 0

#define MTREE_MNODE_N 1024
struct mtree
{
    mtree *active;
    int used;
    mnode nodes[MTREE_MNODE_N];
    mtree *next;
};

#define MNODE_BUFSIZE 16384

#if defined(_MSC_VER) || defined(__GNUC__)
# define INLINE __inline
#else
# define INLINE
#endif

int n_mnode_new = 0;
int n_mnode_delete = 0;

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

INLINE static mnode *
mnode_new(mtree *mt)
{
    mtree *active = mt->active;

    if (active->used >= MTREE_MNODE_N)
    {
        mtree *p = (mtree *)calloc(1, sizeof(*active->next));
        if (!p)
            return NULL;
        active->next = p;
        mt->active = p;
        active = p;
    }
    ++n_mnode_new;
    return &active->nodes[active->used++];
}

static void
mnode_delete(mnode *p)
{
    while (p)
    {
        mnode *child = p->child;

        if (p->list)
            wordlist_destroy(p->list);
        if (p->low)
            mnode_delete(p->low);
        if (p->high)
            mnode_delete(p->high);
        p = child;
        ++n_mnode_delete;
    }
}

void
mnode_print_stub(mnode *vp, unsigned char *p)
{
    static unsigned char buf[256];

    if (!vp)
        return;
    if (!p)
        p = &buf[0];
    p[0] = MNODE_GET_CH(vp);
    p[1] = '\0';
    if (vp->list)
        printf("%s (list=%p)\n", buf, vp->list);
    if (vp->child)
        mnode_print_stub(vp->child, p + 1);
    if (vp->low)
        mnode_print_stub(vp->low, p);
    if (vp->high)
        mnode_print_stub(vp->high, p);
}

void
mnode_print(mtree *mt, unsigned char *p)
{
    if (mt && mt->used > 0)
        mnode_print_stub(&mt->nodes[0], p);
}

void
mnode_close(mtree *mt)
{
    if (mt)
    {
        mtree *next;

        if (mt->used > 0)
            mnode_delete(&mt->nodes[0]);

        while (mt)
        {
            next = mt->next;
            free(mt);
            mt = next;
        }
    }
}

INLINE static mnode *
search_or_new_mnode(mtree *mt, wordbuf *buf)
{
    // Add to the search tree once the label word is determined
    int ch;
    unsigned char *word;
    mnode **ppnext;
    mnode **res = NULL; // To suppress warning for GCC
    mnode *root;

    word = WORDBUF_GET(buf);
    root = mt->used > 0 ? &mt->nodes[0] : NULL;
    ppnext = &root;
    while ((ch = *word) != 0)
    {
        res = ppnext;
        if (!*res)
        {
            *res = mnode_new(mt);
            if (!(*res))
                return NULL;
            MNODE_SET_CH(*res, ch);
        }
        else
        {
            int pivot = MNODE_GET_CH(*res);
            if (ch < pivot)
            {
                ppnext = &(*res)->low;
                continue;
            }
            else if (ch > pivot)
            {
                ppnext = &(*res)->high;
                continue;
            }
        }
        ppnext = &(*res)->child;
        ++word;
    }

    return *res;
}

// Batch add data from a file to existing nodes.
mtree *
mnode_load(mtree *mt, FILE *fp)
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
    if (mt && mt->used > 0)
    {
        mnode_stat st;
        mnode_debug_stat(&mt->nodes[0], &st);
        mnode_print_stat(&st);
    }
#endif
    return mt;
}

mtree *
mnode_open(FILE *fp)
{
    mtree *mt;

    mt = (mtree *)calloc(1, sizeof(*mt));
    mt->active = mt;
    if (mt && fp)
        if (!mnode_load(mt, fp))
        {
            mnode_close(mt);
            return NULL;
        }

    return mt;
}

#if 0
    static int
mnode_size(mnode* p)
{
    return p ? mnode_size(p->child) + mnode_size(p->next) + 1 : 0;
}
#endif

static mnode *
mnode_query_stub(mnode *node, const unsigned char *query)
{
    int pivot = MNODE_GET_CH(node);

    if (*query < pivot)
        return node->low ? mnode_query_stub(node->low, query) : NULL;
    else if (*query > pivot)
        return node->high ? mnode_query_stub(node->high, query) : NULL;
    else
    {
        if (*++query == '\0')
            return node;
        return node->child ? mnode_query_stub(node->child, query) : NULL;
    }
}

mnode *
mnode_query(mtree *mt, const unsigned char *query)
{
    return (query && *query != '\0' && mt)
                   ? mnode_query_stub(&mt->nodes[0], query)
                   : 0;
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
