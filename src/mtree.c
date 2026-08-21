// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// mtree.c - Migemo tree (mtree) operations
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "mtree.h"
#include "strbuf.h"
#include "trie.h"
#include "wordlist.h"

#define MNODE_BUFSIZE 16384

#define MNODE_BLOCK_BYTES       (64 * 1024)
#define MNODE_BLOCK_HEADER_SIZE (sizeof(void *) + sizeof(size_t))
#define MNODE_BLOCK_SIZE                                                       \
    ((MNODE_BLOCK_BYTES - MNODE_BLOCK_HEADER_SIZE) / sizeof(mnode))

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
};

static void
mnode_debug_stat_stub(
        mnode *node, trie_stat *stat, int sib_depth, int total_cmp)
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
        stat->wordtail_count++;
        stat->total_node_cmp_count += total_cmp;
        if (total_cmp > stat->max_node_cmp_count)
            stat->max_node_cmp_count = total_cmp;
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
mnode_debug_stat(mnode *root, trie_stat *stat)
{
    if (!stat)
        return;
    memset(stat, 0, sizeof(*stat));
    if (!root)
        return;
    mnode_debug_stat_stub(root, stat, 0, 1);
}

void
mtree_print_stat(mtree *mt, const char *label)
{
    if (!mt || !mt->rootnode)
    {
        printf("== INVALID mnode ===\n");
        return;
    }

    trie_stat stat;
    mnode_debug_stat(mt->rootnode, &stat);

    trie_stat_print(&stat, label);
}

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
    p->code = code;
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
mtree_close(mtree *mt)
{
    if (mt)
    {
        mnode_arena_free(&mt->arena);
        free(mt);
    }
}

static inline mnode *
mtree_ensure_node(mtree *mt, const unsigned char *key)
{
    if (!key)
        return NULL;
    unsigned int code = charset_decode(mt->char2int, &key);
    if (code == 0)
        return NULL;

    mnode **res = NULL;
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
            unsigned int pivot = (*res)->code;
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
        (*res)->weight++;
        code = charset_decode(mt->char2int, &key);
        if (code == 0)
            break;
    }

    return *res;
}

size_t
mnode_count_siblings(mnode *node)
{
    if (!node)
        return 0;
    return mnode_count_siblings(node->low) + 1
           + mnode_count_siblings(node->high);
}

mnode **
mnode_collect_siblings(mnode *node, mnode **buf)
{
    if (!node)
        return buf;
    buf = mnode_collect_siblings(node->low, buf);
    *buf++ = node;
    return mnode_collect_siblings(node->high, buf);
}

static mnode *
mnode_balanced_tree(mnode **nodes, size_t start, size_t end)
{
    if (start >= end)
        return NULL;

    int left = (int)start - 1, right = (int)end;
    unsigned int lsum = 0, rsum = 0;
    while (left < right)
        if (lsum < rsum
                || (lsum == rsum && (left - start + 1) <= (end - 1 - right)))
            lsum += nodes[++left]->weight;
        else
            rsum += nodes[--right]->weight;
    size_t mid = left;

    mnode *root = nodes[mid];
    root->low = mnode_balanced_tree(nodes, start, mid);
    root->high = mnode_balanced_tree(nodes, mid + 1, end);
    return root;
}

static mnode *
mnode_balance(mnode *root)
{
    if (!root)
        return NULL;
    size_t count = mnode_count_siblings(root);
    mnode **nodes = calloc(count, sizeof(mnode *));
    mnode_collect_siblings(root, nodes);
    root = mnode_balanced_tree(nodes, 0, count);
    for (size_t i = 0; i < count; i++)
        nodes[i]->child = mnode_balance(nodes[i]->child);
    free(nodes);
    return root;
}

static inline int
mtree_scan_to_next_break(
        unsigned char **start, unsigned char **end, unsigned char **in)
{
    for (unsigned char *p = *in; *p; p++)
    {
        if (*p < 0x80 && isspace(*p))
            continue;
        *start = p;
        for (; *p; p++)
        {
            if (*p == '\t')
            {
                *p = '\0';
                *end = p;
                *in = p + 1;
                return 0;
            }
        }
        *end = p;
        *in = p;
        return 2; // EOL
    }
    return 1; // SKIP: empty line.
}

typedef struct
{
    FILE *fp;
    size_t avail;
    size_t head;
    int err;
    bool eof;
    bool read_request;
    unsigned char buf[MNODE_BUFSIZE];
} mtree_file;

static inline unsigned char *
mtree_readline(mtree_file *mf, size_t *line_len)
{
    while (1)
    {
        if (mf->read_request)
        {
            size_t len = fread(mf->buf + mf->avail, 1,
                    sizeof(mf->buf) - mf->avail - 1, mf->fp);
            mf->err = ferror(mf->fp);
            mf->eof = feof(mf->fp);
            if (len == 0 || mf->err != 0)
                return NULL;
            mf->head = 0;
            mf->avail += len;
            mf->read_request = false;
        }

        unsigned char *start = mf->buf + mf->head;

        unsigned char *tail = memchr(start, '\n', mf->avail - mf->head);
        if (tail)
        {
            size_t len = tail - start + 1;
            mf->head += len;
            *line_len = len;
            return start;
        }

        if (mf->eof)
        {
            // Reached EOF.
            size_t len = mf->avail - mf->head;
            mf->head = mf->avail;
            *line_len = len;
            mf->buf[mf->avail] = '\0';
            return len != 0 ? start : NULL;
        }

        // Shift remaining data to the front to read more.
        size_t len = mf->avail - mf->head;
        memmove(mf->buf, mf->buf + mf->head, len);
        mf->avail = len;
        mf->head = 0;
        mf->read_request = true;
    }

    return NULL;
}

// Batch add data from a file to existing nodes.
mtree *
mtree_load(mtree *mt, FILE *fp, CHARSET_PROC_CHAR2INT char2int)
{
    mt->char2int =
            char2int && char2int != charset_utf8_char2int ? char2int : NULL;

    mtree_file mf = {.fp = fp, .read_request = true};
    while (1)
    {
        // Read a line from the file.
        size_t len = 0;
        unsigned char *p = mtree_readline(&mf, &len);
        if (!p)
        {
            if (mf.eof)
                break;
            // ERROR: File read error.
            return NULL;
        }
        if (len > 0 && p[len - 1] != '\n' && !mf.eof)
        {
            // ERROR: Line is too long.
            return NULL;
        }
        // Trim last LF ('\n').
        if (len > 0 && p[len - 1] == '\n')
            p[len - 1] = '\0';

        // Skip the comment line
        if (*p == ';')
            continue;

        unsigned char *key = NULL, *kend = NULL;
        switch (mtree_scan_to_next_break(&key, &kend, &p))
        {
            case 0: // found a key, forward to the values.
                break;
            case 1: // skip empty line, forward to the next line.
                continue;
            case 2: // error
                return NULL;
        }

        wordlist *values = NULL, **tail = &values;
        while (1)
        {
            unsigned char *value = NULL, *vend = NULL;
            int ret = mtree_scan_to_next_break(&value, &vend, &p);
            if (value)
            {
                *tail = wordlist_new(value, vend - value);
                tail = &(*tail)->next;
            }
            // Cases where the loop is exited
            //   ret == 1: couldn't found any values.
            //   ret == 2: EOL, found values.
            if (ret != 0)
                break;
        }
        if (!values)
            continue;

        // Register a key with values.
        mnode *node = mtree_ensure_node(mt, key);
        if (!node)
        {
            wordlist_destroy(values);
            return NULL;
        }
        *tail = node->list;
        node->list = values;
    }

    mt->rootnode = mnode_balance(mt->rootnode);
    return mt;
}

mtree *
mtree_open(void)
{
    mtree *mt = (mtree *)calloc(1, sizeof(*mt));
    // Set the default to UTF-8.
    mt->char2int = NULL;
    return mt;
}

mnode *
mtree_query(mtree *mt, const unsigned char *query)
{
    if (!mt || !mt->rootnode || !query)
        return NULL;

    unsigned int code = charset_decode(mt->char2int, &query);
    if (code == 0)
        return NULL;
    mnode *node = mt->rootnode;

    while (node)
    {
        // Search from siblings
        while (node != NULL && code != node->code)
            node = code < node->code ? node->low : node->high;
        if (!node)
            break; // Not found the rune.
        // Proceed to the child node
        code = charset_decode(mt->char2int, &query);
        if (code == 0)
            break; // Found the node.
        node = node->child;
    }
    return node;
}
