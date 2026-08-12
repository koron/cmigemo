// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji.c - Romaji conversion
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "romaji.h"
#include "trie.h"
#include "wordbuf.h"

#if defined(_MSC_VER) || defined(__GNUC__)
# define INLINE __inline
#else
# define INLINE
#endif

#ifdef _DEBUG
// clang-format off
# define VERBOSE(o,l,b)     if ((o)->verbose >= (l)) { b }
// clang-format on
#else
# define VERBOSE(o, l, b)
#endif

#if defined(_MSC_VER)
# define STRDUP _strdup
#else
# define STRDUP strdup
#endif

#define ROMAJI_FIXKEY_N      'n'
#define ROMAJI_FIXKEY_XN     "xn"
#define ROMAJI_FIXKEY_XTU    "xtu"
#define ROMAJI_FIXKEY_NONXTU "aiueon"

// romanode interfaces

#define ROMANODE_BLOCK_SIZE 1024

typedef struct romanode romanode;
struct romanode
{
    romanode *low, *high;
    romanode *child;

    unsigned int code;
    unsigned char key;
    unsigned char *value;
    unsigned char *remain;
};

typedef struct romanode_block romanode_block;
struct romanode_block
{
    romanode_block *next;
    size_t used;
    romanode nodes[ROMANODE_BLOCK_SIZE];
};

typedef struct romanode_arena romanode_arena;
struct romanode_arena
{
    romanode_block *head;
    romanode_block *curr;
};

struct romaji
{
    int verbose;

    romanode *rootnode;
    romanode_arena arena;

    unsigned char *fixvalue_xn;
    unsigned char *fixvalue_xtu;
    ROMAJI_PROC_CHAR2INT char2int;
};

static romanode *
romanode_arena_alloc(romanode_arena *arena, unsigned int code)
{
    if (!arena->curr || arena->curr->used >= ROMANODE_BLOCK_SIZE)
    {
        romanode_block *block =
                (romanode_block *)calloc(1, sizeof(romanode_block));
        if (!block)
            return NULL;
        if (!arena->head)
            arena->head = block;
        else
            arena->curr->next = block;
        arena->curr = block;
    }
    romanode *p = &arena->curr->nodes[arena->curr->used++];
    p->code = code;
    return p;
}

static void
romanode_arena_free(romanode_arena *arena)
{
    for (romanode_block *p = arena->head; p;)
    {
        romanode_block *next = p->next;
        for (int i = 0; i < p->used; i++)
        {
            free(p->nodes[i].value);
            free(p->nodes[i].remain);
        }
        free(p);
        p = next;
    }
    arena->head = NULL;
    arena->curr = NULL;
}

static romanode *
romanode_dig(romanode_arena *arena, romanode **pp, unsigned int code)
{
    romanode *p = *pp;
    if (p == NULL)
    {
        *pp = romanode_arena_alloc(arena, code);
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
                p->low = romanode_arena_alloc(arena, code);
                return p->low;
            }
            p = p->low;
        }
        else
        {
            if (p->high == NULL)
            {
                p->high = romanode_arena_alloc(arena, code);
                return p->high;
            }
            p = p->high;
        }
    }
}

static void
romanode_balance(romanode **pnode)
{
    if (!pnode || !*pnode)
        return;
    // TODO: balance siblings
    // TODO: do same for descedants
}

/// Search for and return the romanode corresponding to the key.
/// @return NULL if romanode is not found
/// @param node root node
/// @param key search key
/// @param skip pointer to receive the number of bytes to skip in key
static romanode *
romanode_query(romanode *node, const unsigned char *key, int *skip,
        ROMAJI_PROC_CHAR2INT char2int)
{
    int nskip = 0;
    const unsigned char *key_start = key;

    // printf("romanode_query: key=%s skip=%p char2int=%p\n", key, skip,
    // char2int);
    if (node && key && *key)
    {
        while (1)
        {
            if (*key != node->key)
                node = *key < node->key ? node->low : node->high;
            else
            {
                ++nskip;
                if (node->value)
                {
                    // printf("  HERE 1\n");
                    break;
                }
                if (!*++key)
                {
                    nskip = 0;
                    // printf("  HERE 2\n");
                    break;
                }
                node = node->child;
            }
            // If the next node to traverse is empty, advance the key and return
            // NULL
            if (!node)
            {
                // Advance by one character, not one byte
                if (!char2int || (nskip = (*char2int)(key_start, NULL)) < 1)
                    nskip = 1;
                // printf("  HERE 3: nskip=%d\n", nskip);
                break;
            }
        }
    }

    if (skip)
        *skip = nskip;
    return node;
}

// romaji interfaces

static unsigned char *
strdup_lower(const unsigned char *string)
{
    unsigned char *out = STRDUP(string), *tmp;

    if (out)
        for (tmp = out; *tmp; ++tmp)
            *tmp = (unsigned char)tolower(*tmp);
    return out;
}

romaji *
romaji_open()
{
    return (romaji *)calloc(1, sizeof(romaji));
}

void
romaji_close(romaji *object)
{
    if (object)
    {
        romanode_arena_free(&object->arena);
        free(object->fixvalue_xn);
        free(object->fixvalue_xtu);
        free(object);
    }
}

static int
romaji_add_entry(romaji *object, const unsigned char *key,
        const unsigned char *value, const unsigned char *remain)
{
    romanode **ppnode = &object->rootnode;
    romanode *pnode = NULL;
    const unsigned char *p = key;
    while (1)
    {
        unsigned int code = charset_decode(object->char2int, &p);

        if (code == 0)
        {
            if (!pnode)
                return 2; // Empty key.

            // Discard longer keys.
            if (*ppnode)
                *ppnode = NULL;

            // Duplicate value, and remain.
            unsigned char *dup_value = STRDUP(value);
            if (!dup_value)
                return 3; // Allocation for value failed.
            unsigned char *dup_remain = NULL;
            if (remain)
            {
                dup_remain = STRDUP(remain);
                if (!dup_remain)
                    return 4; // Allocation for remain failed.
            }

            // Replace existing value, and remain.
            free(pnode->value);
            free(pnode->remain);
            pnode->value = dup_value;
            pnode->remain = dup_remain;
            break;
        }
        pnode = romanode_dig(&object->arena, ppnode, code);
        if (!pnode)
            return 5; // Allocation error.

        // Move the focus deeper by traversing child nodes
        ppnode = &pnode->child;

        // Do not add the key if a shorter key is found.
        if (pnode && pnode->value)
            return 1;
    }

    return 0;
}

#define ROMAJI_READ_BUFSIZE     1024
#define ROMAJI_PUSHBACK_BUFSIZE 1024

typedef enum {
    MODE_KEY_WAITING = 0,
    MODE_KEY_READING = 1,
    MODE_VALUE_WAITING = 2,
    MODE_VALUE_READING = 3,
    MODE_REMAIN_WAITING = 4,
    MODE_REMAIN_READING = 5,
    MODE_LINE_SKIP = 6,
} load_mode;

static int
isspace_u(unsigned int code)
{
    return code < 0x80 && isspace((int)code);
}

static int
romaji_load_stub(romaji *object, FILE *fp)
{
    unsigned char buf[ROMAJI_READ_BUFSIZE];
    while (1)
    {
        // Read a line from the file.
        if (!fgets(buf, sizeof(buf), fp))
        {
            if (feof(fp))
                break;
            // File read error.
            return 1;
        }
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n' && !feof(fp))
        {
            // The line is too long.
            return 2;
        }

        // Parse the line.
        load_mode mode = MODE_KEY_WAITING;
        unsigned char *end = buf + len;
        unsigned char *next;
        unsigned char *key = NULL, *key_end = NULL;
        unsigned char *value = NULL, *value_end = NULL;
        unsigned char *remain = NULL, *remain_end = NULL;
        for (unsigned char *p = buf; p < end; p = next)
        {
            next = p;
            unsigned int code = charset_decode(
                    object->char2int, (const unsigned char **)&next);

            // End of the line.
            if (code == '\n' || code == 0)
            {
                // Skip an empty line.
                if (!key)
                    break;
                // Syntax error: value missing.
                if (!value)
                    return 3;

                if (!value_end)
                    // Case of: Key + Value + EOL
                    value_end = p;
                else if (remain && !remain_end)
                    // Case of: Key + Value + Remain
                    remain_end = p;
                // Add a valid node to the dictionary.
                *key_end = '\0';
                *value_end = '\0';
                if (remain_end)
                    *remain_end = '\0';
                int err = romaji_add_entry(object, key, value, remain);
                if (err > 1)
                    return err * 10 + 4;
                break;
            }

            switch (mode)
            {
                case MODE_KEY_WAITING:
                    if (code == '#')
                    {
                        // If the line starts with `##`, the key is treated as
                        // starting with `#`; if it starts with only `#`, it is
                        // treated as a comment line.
                        unsigned char *next2 = next;
                        unsigned int code2 = charset_decode(object->char2int,
                                (const unsigned char **)&next2);
                        if (code2 != '#')
                        {
                            next = end;
                            continue;
                        }
                        code = code2;
                        p = next;
                        next = next2;
                    }
                    if (!isspace_u(code))
                    {
                        key = p;
                        mode = MODE_KEY_READING;
                    }
                    break;

                case MODE_KEY_READING:
                    if (isspace_u(code))
                    {
                        key_end = p;
                        mode = MODE_VALUE_WAITING;
                    }
                    break;

                case MODE_VALUE_WAITING:
                    if (!isspace_u(code))
                    {
                        value = p;
                        mode = MODE_VALUE_READING;
                    }
                    break;

                case MODE_VALUE_READING:
                    if (isspace_u(code))
                    {
                        value_end = p;
                        mode = MODE_REMAIN_WAITING;
                    }
                    break;

                case MODE_REMAIN_WAITING:
                    if (!isspace_u(code))
                    {
                        remain = p;
                        mode = MODE_REMAIN_READING;
                    }
                    break;

                case MODE_REMAIN_READING:
                    if (isspace_u(code))
                    {
                        remain_end = p;
                        mode = MODE_LINE_SKIP;
                    }
                    break;

                case MODE_LINE_SKIP:
                default:
                    break;
            }
        }
    }
    return 0;
}

static void
romanode_stat_stub(
        romanode *node, trie_stat *stat, int sib_depth, int total_cmp)
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

    if (node->value)
    {
        stat->wordtail_count++;
        stat->total_node_cmp_count += total_cmp;
        if (total_cmp > stat->max_node_cmp_count)
            stat->max_node_cmp_count = total_cmp;
    }

    // recursive calls for low, high, and child nodes.
    if (node->low)
        romanode_stat_stub(node->low, stat, sib_depth + 1, total_cmp + 1);
    if (node->high)
        romanode_stat_stub(node->high, stat, sib_depth + 1, total_cmp + 1);
    if (node->child)
        romanode_stat_stub(node->child, stat, 0, total_cmp + 1);
}

static void
romanode_stat(romaji *obj, trie_stat *stat)
{
    if (!stat || !obj || !obj->rootnode)
        return;
    memset(stat, 0, sizeof(*stat));
    romanode_stat_stub(obj->rootnode, stat, 0, 1);
}

void
romanode_print_stat(romaji *obj, const char *title)
{
    if (!obj || !obj->rootnode)
    {
        printf("== INVALID romanode ===\n");
        return;
    }
    trie_stat stat;
    romanode_stat(obj, &stat);
    trie_stat_print(&stat, title);
}

/// Load the Romaji dictionary.
/// @param object Romaji object
/// @param filename Dictionary filename
/// @return 0 on success, non-zero on failure.
int
romaji_load(romaji *object, const unsigned char *filename,
        CHARSET_PROC_CHAR2INT char2int)
{
    if (!object || !filename)
        return -1;
    object->char2int = char2int;
    FILE *fp = fopen(filename, "rt");
    if (!fp)
        return -1;
    int result = romaji_load_stub(object, fp);
    fclose(fp);
    romanode_balance(&object->rootnode);
    return result;
}

unsigned char *
romaji_convert2(romaji *object, const unsigned char *string,
        unsigned char **ppstop, int ignorecase)
{
    // Argument "ppstop" receive conversion stoped position.
    wordbuf *buf = NULL;
    unsigned char *lower = NULL;
    unsigned char *answer = NULL;
    const unsigned char *input = string;
    int stop = -1;

    if (ignorecase)
    {
        lower = strdup_lower(string);
        input = lower;
    }

    if (object && string && input && (buf = wordbuf_open()))
    {
        int i;

        for (i = 0; string[i];)
        {
            romanode *node;
            int skip;

            // Detect "tsu" (small tsu: "っ")
            if (object->fixvalue_xtu && input[i] == input[i + 1]
                    && !strchr(ROMAJI_FIXKEY_NONXTU, input[i]))
            {
                ++i;
                wordbuf_cat(buf, object->fixvalue_xtu);
                continue;
            }

            node = romanode_query(
                    object->rootnode, &input[i], &skip, object->char2int);
            VERBOSE(object, 1,
                    printf("key=%s value=%s skip=%d\n", &input[i],
                            node && node->value ? (char *)node->value : "null",
                            skip);)
            if (skip == 0)
            {
                if (string[i])
                {
                    stop = (int)WORDBUF_LEN(buf);
                    wordbuf_cat(buf, &string[i]);
                }
                break;
            }
            else if (!node)
            {
                // Convert "n + (consonant)" to "ん + (consant)"
                if (skip == 1 && input[i] == ROMAJI_FIXKEY_N
                        && object->fixvalue_xn)
                {
                    ++i;
                    wordbuf_cat(buf, object->fixvalue_xn);
                }
                else
                    while (skip--)
                        wordbuf_add(buf, string[i++]);
            }
            else
            {
                i += skip;
                wordbuf_cat(buf, node->value);
            }
        }
        answer = STRDUP(WORDBUF_GET(buf));
    }
    if (ppstop)
        *ppstop = (stop >= 0) ? answer + stop : NULL;

    if (lower)
        free(lower);
    if (buf)
        wordbuf_close(buf);
    return answer;
}

unsigned char *
romaji_convert(
        romaji *object, const unsigned char *string, unsigned char **ppstop)
{
    return romaji_convert2(object, string, ppstop, 1);
}

void
romaji_release(romaji *object, unsigned char *string)
{
    free(string);
}

void
romaji_setproc_char2int(romaji *object, ROMAJI_PROC_CHAR2INT proc)
{
    if (object)
        object->char2int = proc;
}

void
romaji_set_verbose(romaji *object, int level)
{
    if (object)
        object->verbose = level;
}

static romanode *
find_siblings(romaji *object, romanode *node, unsigned int code)
{
    if (node && node->child)
        node = node->child;
    if (!node)
        node = object->rootnode;

    while (node != NULL)
    {
        if (node->code == code)
            return node;
        else if (code < node->code)
            node = node->low;
        else
            node = node->high;
    }
    return NULL;
}

static inline size_t
decode_len(CHARSET_PROC_CHAR2INT proc, const unsigned char *s)
{
    int len = proc(s, NULL);
    return len > 0 ? (size_t)len : 1;
}

static wordlist *
add_pending_node(wordlist *tail, romanode *node, wordbuf *prefix)
{
    if (!node)
        return tail;
    tail = add_pending_node(tail, node->low, prefix);
    if (node->value)
    {
        wordbuf *w = wordbuf_open();
        if (w)
        {
            wordbuf_append(w, prefix);
            wordbuf_cat(w, node->value);
            wordlist *item = wordlist_new(w->buf, w->last);
            tail->next = item;
            tail = item;
            wordbuf_close(w);
        }
    }
    tail = add_pending_node(tail, node->child, prefix);
    tail = add_pending_node(tail, node->high, prefix);
    return tail;
}

static void
add_pending_node_all(wordlist *tail, romanode *node, wordbuf *prefix)
{
    add_pending_node(tail, node->child, prefix);
}

wordlist *romaji_convert_all(romaji *object, const unsigned char *src)
{
    wordlist *list = NULL;
    unsigned char *srcbuf = NULL;
    wordbuf *dstbuf = NULL;
    wordbuf *pending = NULL;

    size_t srclen = strlen(src);
    srcbuf = calloc(1, ROMAJI_PUSHBACK_BUFSIZE + srclen + 1);
    if (!srcbuf)
        goto END;
    unsigned char *curr = srcbuf + ROMAJI_PUSHBACK_BUFSIZE;
    memcpy(curr, src, srclen + 1);

    dstbuf = wordbuf_open();
    if (!dstbuf)
        goto END;
    pending = wordbuf_open();
    if (!pending)
        goto END;

    romanode *node = NULL;
    while (1)
    {
        const unsigned char *prev = curr;
        unsigned int code =
                charset_decode(object->char2int, (const unsigned char **)&curr);
        if (code == 0)
            break;

        node = find_siblings(object, node, code);

        if (!node)
        {
            wordbuf_write_bytes(pending, prev, curr - prev);
            // Consume a code from the pending, add it to dstbuf.
            size_t len = decode_len(object->char2int, pending->buf);
            wordbuf_write_bytes(dstbuf, pending->buf, len);
            // Push the pending remainder to the front of srcbuf.
            size_t rem_len = pending->last - len;
            if (curr < srcbuf + rem_len) // Check if srcbuf has underflowed.
                goto END;
            curr -= rem_len;
            memcpy(curr, pending->buf + len, rem_len);
            wordbuf_reset(pending);
            // Start the next search from the root.
            node = NULL;
            continue;
        }

        if (node->value)
        {
            wordbuf_cat(dstbuf, node->value);
            // Push node->remain to front of curr.
            if (node->remain)
            {
                size_t len = strlen(node->remain);
                // Check for source buffer underflow.
                if (curr < srcbuf + len)
                    goto END;
                curr -= len;
                memcpy(curr, node->remain, len);
            }
            wordbuf_reset(pending);
            // Start the next search from the root.
            node = NULL;
            continue;
        }

        wordbuf_write_bytes(pending, prev, curr - prev);
    }

    // Output all entries under the pending node.
    wordlist pendings = {0};
    if (node && node != object->rootnode)
        add_pending_node_all(&pendings, node, dstbuf);

    if (pending->last > 0)
        wordbuf_append(dstbuf, pending);
    list = wordlist_new(dstbuf->buf, dstbuf->last);
    if (!list)
    {
        wordlist_destroy(pendings.next);
        goto END;
    }
    list->next = pendings.next;

END:
    free(srcbuf);
    wordbuf_close(dstbuf);
    wordbuf_close(pending);
    return list;
}
