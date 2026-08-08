// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji.c - Romaji conversion
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "romaji.h"
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
    unsigned char key;
    romanode *next;
    romanode *child;

    unsigned char *value;
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

int n_romanode_new = 0;

static romanode *
romanode_arena_alloc(romanode_arena *arena, unsigned char key)
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
    p->key = key;
    ++n_romanode_new; // FIXME: record to arena
    return p;
}

static void
romanode_arena_free(romanode_arena *arena)
{
    for (romanode_block *p = arena->head; p;)
    {
        romanode_block *next = p->next;
        for (int i = 0; i < p->used; i++)
            if (p->nodes[i].value)
                free(p->nodes[i].value);
        free(p);
        p = next;
    }
    arena->head = NULL;
    arena->curr = NULL;
}

static romanode **
romanode_dig(
        romanode_arena *arena, romanode **ref_node, const unsigned char *key)
{
    if (!ref_node || !key || key[0] == '\0')
        return NULL;

    while (1)
    {
        if (!*ref_node)
            if (!(*ref_node = romanode_arena_alloc(arena, *key)))
                return NULL;

        if ((*ref_node)->key == *key)
        {
            (*ref_node)->value = NULL;
            if (!*++key)
                break;
            ref_node = &(*ref_node)->child;
        }
        else
            ref_node = &(*ref_node)->next;
    }

    // If a key shorter than an existing Romaji conversion key is registered,
    // the node for the longer key is discarded as invalid.  The `value` field
    // of the existing node being detached here is deallocated precisely when
    // the arena is freed.
    (*ref_node)->child = NULL;

    return ref_node;
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
                node = node->next;
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

struct romaji
{
    int verbose;

    romanode *node;
    romanode_arena arena;

    unsigned char *fixvalue_xn;
    unsigned char *fixvalue_xtu;
    ROMAJI_PROC_CHAR2INT char2int;
};

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

int
romaji_add_table(
        romaji *object, const unsigned char *key, const unsigned char *value)
{
    size_t value_length;
    romanode **ref_node;

    if (!object || !key || !value)
        return 1; // Unexpected error

    value_length = strlen(value);
    if (value_length == 0)
        return 2; // Too short value string

    if (!(ref_node = romanode_dig(&object->arena, &object->node, key)))
    {
        return 4; // Memory exhausted
    }
    VERBOSE(object, 10,
            printf("romaji_add_table(\"%s\", \"%s\")\n", key, value););
    (*ref_node)->value = STRDUP(value);

    // Save "n" ("ん") and "tsu" ("っ")
    if (object->fixvalue_xn == NULL && value_length > 0
            && !strcmp(key, ROMAJI_FIXKEY_XN))
    {
        // fprintf(stderr, "XN: key=%s, value=%s\n", key, value);
        object->fixvalue_xn = STRDUP(value);
    }
    if (object->fixvalue_xtu == NULL && value_length > 0
            && !strcmp(key, ROMAJI_FIXKEY_XTU))
    {
        // fprintf(stderr, "XTU: key=%s, value=%s\n", key, value);
        object->fixvalue_xtu = STRDUP(value);
    }

    return 0;
}

int
romaji_load_stub(romaji *object, FILE *fp)
{
    int mode, ch;
    wordbuf *buf_key;
    wordbuf *buf_value;

    buf_key = wordbuf_open();
    buf_value = wordbuf_open();
    if (!buf_key || !buf_value)
    {
        wordbuf_close(buf_key);
        wordbuf_close(buf_value);
        return -1;
    }

    mode = 0;
    do
    {
        ch = fgetc(fp);
        switch (mode)
        {
            case 0:
                // Waiting for key mode
                if (ch == '#')
                {
                    // If the next character is whitespace, treat it as part of
                    // the key
                    ch = fgetc(fp);
                    if (ch != '#')
                    {
                        ungetc(ch, fp);
                        mode = 1; // Transition to skipping until end of line
                                  // mode
                        break;
                    }
                }
                if (ch != EOF && !isspace(ch))
                {
                    wordbuf_reset(buf_key);
                    wordbuf_add(buf_key, (unsigned char)ch);
                    mode = 2; // Transition to key reading mode
                }
                break;

            case 1:
                // Skipping until end of line mode
                if (ch == '\n')
                    mode = 0; // Transition to waiting for key mode
                break;

            case 2:
                // Key reading mode
                if (!isspace(ch))
                    wordbuf_add(buf_key, (unsigned char)ch);
                else
                    mode = 3; // Transition to waiting for value mode
                break;

            case 3:
                // Waiting for value mode
                if (ch != EOF && !isspace(ch))
                {
                    wordbuf_reset(buf_value);
                    wordbuf_add(buf_value, (unsigned char)ch);
                    mode = 4; // Transition to value reading mode
                }
                break;

            case 4:
                // Value reading mode
                if (ch != EOF && !isspace(ch))
                    wordbuf_add(buf_value, (unsigned char)ch);
                else
                {
                    unsigned char *key = WORDBUF_GET(buf_key);
                    unsigned char *value = WORDBUF_GET(buf_value);
                    romaji_add_table(object, key, value);
                    mode = 0;
                }
                break;
        }
    }
    while (ch != EOF);

    wordbuf_close(buf_key);
    wordbuf_close(buf_value);
    return 0;
}

/// Load the Romaji dictionary.
/// @param object Romaji object
/// @param filename Dictionary filename
/// @return 0 on success, non-zero on failure.
int
romaji_load(romaji *object, const unsigned char *filename)
{
    FILE *fp;
    int charset;
    if (!object || !filename)
        return -1;
#if 1
    charset = charset_detect_file(filename);
    charset_getproc(charset, (CHARSET_PROC_CHAR2INT *)&object->char2int, NULL);
#endif
    if ((fp = fopen(filename, "rt")) != NULL)
    {
        int result = result = romaji_load_stub(object, fp);
        fclose(fp);
        return result;
    }
    else
        return -1;
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
                    object->node, &input[i], &skip, object->char2int);
            VERBOSE(object, 1,
                    printf("key=%s value=%s skip=%d\n", &input[i],
                            node && node->value ? (char *)node->value : "null",
                            skip);)
            if (skip == 0)
            {
                if (string[i])
                {
                    stop = WORDBUF_LEN(buf);
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
