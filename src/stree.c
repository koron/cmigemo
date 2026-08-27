// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// stree.c - Static version of mtree

#include "common.h"

#include <stdlib.h>
#include <string.h>

#include "mtree.h"
#include "stree.h"

//////////////////////////////////////////////////////////////////////////////

static stree *
stree_alloc(stree_header *head)
{
    if (head->node_count == 0)
        return NULL;
    stree *st = calloc(1, sizeof(stree));
    if (!st)
        goto FAIL;
    st->head = *head;
    st->nodes = calloc(head->node_count, sizeof(snode));
    st->word_buf = calloc(head->word_buf_size, sizeof(uint8_t));
    if (!st->nodes || !st->word_buf)
        goto FAIL;
    return st;
FAIL:
    stree_destroy(st);
    return NULL;
}

void
stree_destroy(stree *st)
{
    if (!st)
        return;
    free(st->nodes);
    free(st->word_buf);
    free(st);
}

static void
stree_count_mnode(stree_header *head, mnode *node)
{
    head->node_count++;
    if (node->low)
        stree_count_mnode(head, node->low);
    if (node->list)
    {
        for (wordlist *list = node->list; list; list = wordlist_next(list))
            head->word_buf_size += (uint32_t)strlen(wordlist_word(list)) + 1;
        head->word_buf_size++; // for terminal '\0'
    }
    if (node->child)
        stree_count_mnode(head, node->child);
    if (node->high)
        stree_count_mnode(head, node->high);
}

typedef struct stree_converter
{
    stree *st;
    uint32_t node_next;
    uint32_t word_buf_next;
} stree_converter;

static uint32_t
stree_count_mnode_siblings(mnode *node)
{
    if (!node)
        return 0;
    return 1 + stree_count_mnode_siblings(node->low)
           + stree_count_mnode_siblings(node->high);
}

static uint32_t stree_fill_mnode_siblings(
        stree_converter *cnv, mnode *node, uint32_t curr);

static void
stree_fill_mnode(stree_converter *cnv, mnode *node, uint32_t curr)
{
    if (!node)
        return;
    uint32_t countSiblings = stree_count_mnode_siblings(node->child);
    uint32_t start = cnv->node_next;
    uint32_t end = start + countSiblings;
    snode dst = {
            .code = node->code,
            .start = start,
            .end = end,
            .word_idx = STREE_INVALID_WORD_IDX,
    };
    cnv->node_next = end;
    if (node->list)
    {
        dst.word_idx = cnv->word_buf_next;
        for (wordlist *list = node->list; list; list = wordlist_next(list))
        {
            unsigned char *word = wordlist_word(list);
            size_t len = strlen(word);
            memcpy(cnv->st->word_buf + cnv->word_buf_next, word, len);
            cnv->word_buf_next += (uint32_t)len;
            cnv->st->word_buf[cnv->word_buf_next++] = '\0';
        }
        cnv->st->word_buf[cnv->word_buf_next++] = '\0';
    }
    cnv->st->nodes[curr] = dst;
    if (!node->child)
        return;
    stree_fill_mnode_siblings(cnv, node->child, start);
}

static uint32_t
stree_fill_mnode_siblings(stree_converter *cnv, mnode *node, uint32_t curr)
{
    if (node->low)
        curr = stree_fill_mnode_siblings(cnv, node->low, curr);
    stree_fill_mnode(cnv, node, curr);
    curr++;
    if (node->high)
        curr = stree_fill_mnode_siblings(cnv, node->high, curr);
    return curr;
}

static uint8_t
stree_char2int_to_charset(CHARSET_PROC_CHAR2INT char2int)
{
    if (char2int == charset_cp932_char2int)
        return CHARSET_CP932;
    else if (char2int == charset_eucjp_char2int)
        return CHARSET_EUCJP;
    else if (char2int == charset_utf8_char2int || char2int == NULL)
        // With the introduction of default values for optimization purposes,
        // NULL has also come to signify UTF-8.
        return CHARSET_UTF8;
    else
        return CHARSET_NONE;
}

stree *
stree_from_mtree(mtree *mt)
{
    stree_header head = {.node_count = 1};
    memcpy(&head.id, STREE_HEAD_ID_V1, sizeof(head.id));
    mnode *mtroot = mtree_rootnode(mt);
    if (mtroot)
        stree_count_mnode(&head, mtroot);
    stree *st = stree_alloc(&head);
    if (!st)
        return NULL;
    st->char2int = charset_regulate_char2int(mt->char2int);
    st->head.charset = stree_char2int_to_charset(mt->char2int);

    stree_converter cnv = {
            .st = st,
            .node_next = 1,
    };
    mnode root = {
            .child = mtroot,
    };
    stree_fill_mnode(&cnv, &root, 0);
    return st;
}

//////////////////////////////////////////////////////////////////////////////

stree *
stree_load(const char *filename)
{
    stree *retval = NULL, *st = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp)
        goto END;

    // Load the header
    stree_header head = {0};
    if (fread(&head, sizeof(head), 1, fp) < 1)
        goto END;
    if (memcmp(head.id, STREE_HEAD_ID_V1, 4) != 0)
        goto END;

    // Allocate buffers
    st = stree_alloc(&head);
    if (!st)
        goto END;

    // Load buffers
    if (fread(st->nodes, sizeof(snode), head.node_count, fp) != head.node_count)
        goto END;
    if (fread(st->word_buf, sizeof(uint8_t), head.word_buf_size, fp)
            != head.word_buf_size)
        goto END;

    // Setup charset.
    CHARSET_PROC_CHAR2INT char2int = NULL;
    charset_getproc(head.charset, &char2int, NULL);
    st->char2int = charset_regulate_char2int(char2int);

    retval = st;
    st = NULL;
END:
    if (st)
        stree_destroy(st);
    if (fp)
        fclose(fp);
    return retval;
}

int
stree_save(stree *st, const char *filename)
{
    int retval = 1;

    FILE *fp = fopen(filename, "wb");
    if (!fp)
        goto END;

    stree_header head = st->head;
    memcpy(&head.id, STREE_HEAD_ID_V1, 4);
    if (fwrite(&head, sizeof(stree_header), 1, fp) != 1)
        goto END;

    if (fwrite(st->nodes, sizeof(snode), head.node_count, fp)
            != head.node_count)
        goto END;
    if (fwrite(st->word_buf, sizeof(uint8_t), head.word_buf_size, fp)
            != head.word_buf_size)
        goto END;

    retval = 0;
END:
    if (fp)
    {
        fclose(fp);
        if (retval != 0)
            remove(filename);
    }
    return retval;
}

static uint32_t
stree_find_siblings(stree *st, uint32_t start, uint32_t end, unsigned int code)
{
    if (start < end
            && (code < st->nodes[start].code || code > st->nodes[end - 1].code))
        return STREE_INVALID_NODE_ID;
    while (start < end)
    {
        uint32_t mid = (start + end) / 2;
        uint32_t pivot = st->nodes[mid].code;
        if (code < pivot)
            end = mid;
        else if (code > pivot)
            start = mid + 1;
        else
            return mid;
    }
    return STREE_INVALID_NODE_ID;
}

snode *
stree_query(
        stree *st, const unsigned char *query)
{
    if (!st || !query)
        return NULL;

    unsigned int code = charset_decode(st->char2int, &query);
    if (code == 0)
        return NULL;

    uint32_t curr = 0;
    while (1)
    {
        snode node = st->nodes[curr];
        // Search from siblings
        curr = stree_find_siblings(st, node.start, node.end, code);
        if (curr == STREE_INVALID_NODE_ID)
            return NULL; // Not found the code (rune).
        // Proceed to the child node
        code = charset_decode(st->char2int, &query);
        if (code == 0)
            break; // Found the node.
    }
    return &st->nodes[curr];
}
