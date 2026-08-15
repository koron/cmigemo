// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// migemo.c -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "filename.h"
#include "migemo.h"
#include "migemo_struct.h"
#include "mtree.h"
#include "romaji.h"
#include "rxgen.h"
#include "strbuf.h"
#include "wordlist.h"

#define DICT_MIGEMO    "migemo-dict"
#define DICT_ROMA2HIRA "roma2hira.dat"
#define DICT_HIRA2KATA "hira2kata.dat"
#define DICT_HAN2ZEN   "han2zen.dat"
#define DICT_ZEN2HAN   "zen2han.dat"

#define BUFLEN_DETECT_CHARSET 4096

#ifdef __BORLANDC__
# define EXPORTS __declspec(dllexport)
#else
# define EXPORTS
#endif

static int
my_strlen(const char *s)
{
    size_t len;

    len = strlen(s);
    return len <= INT_MAX ? (int)len : INT_MAX;
}

static mtree *
load_mtree_dictionary(migemo *obj, const char *dict_file)
{
    if (obj->charset == CHARSET_NONE)
        obj->charset = charset_detect_file(dict_file);

    CHARSET_PROC_CHAR2INT char2int = NULL;
    CHARSET_PROC_INT2CHAR int2char = NULL;
    charset_getproc(obj->charset, &char2int, &int2char);
    migemo_setproc_char2int(obj, (MIGEMO_PROC_CHAR2INT)char2int);
    migemo_setproc_int2char(obj, (MIGEMO_PROC_INT2CHAR)int2char);
    obj->char2int = char2int;

    FILE *fp = fopen(dict_file, "rt");
    if (!fp)
        return NULL;
    mtree *mt = mtree_load(obj->mtree, fp, char2int);
    fclose(fp);
    return mt;
}

// migemo interfaces

/// Add a dictionary or data file to the Migemo object.
///
/// Supported dictionary types (`dict_id`):
///
// clang-format off
/// - `MIGEMO_DICTID_MIGEMO`: Main dictionary file (`migemo-dict`).
/// - `MIGEMO_DICTID_ROMA2HIRA`: Romaji to Hiragana conversion table (`roma2hira.dat`).
/// - `MIGEMO_DICTID_HIRA2KATA`: Hiragana to Katakana conversion table (`hira2kata.dat`).
/// - `MIGEMO_DICTID_HAN2ZEN`: Half-width to Full-width conversion table (`han2zen.dat`).
/// - `MIGEMO_DICTID_ZEN2HAN`: Full-width to Half-width conversion table (`zen2han.dat`).
// clang-format on
///
/// @param obj Migemo object.
/// @param dict_id Identifier specifying the type of dictionary/data.
/// @param dict_file Path to the dictionary or data file to load.
/// @return Non-zero on success, 0 on failure.
EXPORTS int MIGEMO_CALLTYPE
migemo_load(migemo *obj, int dict_id, const char *dict_file)
{
    if (!obj && dict_file)
        return MIGEMO_DICTID_INVALID;

    if (dict_id == MIGEMO_DICTID_MIGEMO)
    {
        // Load migemo dictionary
        mtree *mt;

        if ((mt = load_mtree_dictionary(obj, dict_file)) == NULL)
            return MIGEMO_DICTID_INVALID;
        obj->mtree = mt;
        obj->enable = 1;
        return dict_id; // Loaded successfully
    }
    else
    {
        romaji *dict;

        switch (dict_id)
        {
            case MIGEMO_DICTID_ROMA2HIRA:
                // Load romaji dictionary
                dict = obj->roma2hira;
                break;
            case MIGEMO_DICTID_HIRA2KATA:
                // Load katakana dictionary
                dict = obj->hira2kata;
                break;
            case MIGEMO_DICTID_HAN2ZEN:
                // Load half-width to full-width dictionary
                dict = obj->han2zen;
                break;
            case MIGEMO_DICTID_ZEN2HAN:
                // Load half-width to full-width dictionary
                dict = obj->zen2han;
                break;
            default:
                dict = NULL;
                break;
        }
        if (dict && romaji_load(dict, dict_file, obj->char2int) == 0)
            return dict_id;
        else
            return MIGEMO_DICTID_INVALID;
    }
}

EXPORTS migemo *MIGEMO_CALLTYPE
migemo_open(const char *dict)
{
    migemo *obj;

    // Construct the Migemo object and its members
    if (!(obj = (migemo *)calloc(1, sizeof(migemo))))
        return obj;
    obj->enable = 0;
    obj->mtree = mtree_open();
    obj->charset = CHARSET_NONE;
    obj->rx = rxgen_open();
    obj->roma2hira = romaji_open();
    obj->hira2kata = romaji_open();
    obj->han2zen = romaji_open();
    obj->zen2han = romaji_open();
    obj->char2int = charset_none_char2int;
    if (!obj->mtree || !obj->rx || !obj->roma2hira || !obj->hira2kata
            || !obj->han2zen || !obj->zen2han)
    {
        migemo_close(obj);
        return obj = NULL;
    }

    // If a default migemo dictionary is specified, also look for romaji and
    // katakana dictionaries
    if (dict)
    {
#ifndef _MAX_PATH
# define _MAX_PATH 1024 // Placeholder value
#endif
        char dir[_MAX_PATH];
        char roma_dict[_MAX_PATH];
        char kata_dict[_MAX_PATH];
        char h2z_dict[_MAX_PATH];
        char z2h_dict[_MAX_PATH];
        const char *tmp;
        mtree *mt;

        filename_directory(dir, _MAX_PATH, dict);
        tmp = strlen(dir) ? dir : ".";
        filename_join(roma_dict, _MAX_PATH, tmp, DICT_ROMA2HIRA);
        filename_join(kata_dict, _MAX_PATH, tmp, DICT_HIRA2KATA);
        filename_join(h2z_dict, _MAX_PATH, tmp, DICT_HAN2ZEN);
        filename_join(z2h_dict, _MAX_PATH, tmp, DICT_ZEN2HAN);

        mt = load_mtree_dictionary(obj, dict);
        if (mt)
        {
            obj->mtree = mt;
            obj->enable = 1;
            romaji_load(obj->roma2hira, roma_dict, obj->char2int);
            romaji_load(obj->hira2kata, kata_dict, obj->char2int);
            romaji_load(obj->han2zen, h2z_dict, obj->char2int);
            romaji_load(obj->zen2han, z2h_dict, obj->char2int);
        }
    }
    return obj;
}

EXPORTS void MIGEMO_CALLTYPE
migemo_close(migemo *obj)
{
    if (obj)
    {
        if (obj->zen2han)
            romaji_close(obj->zen2han);
        if (obj->han2zen)
            romaji_close(obj->han2zen);
        if (obj->hira2kata)
            romaji_close(obj->hira2kata);
        if (obj->roma2hira)
            romaji_close(obj->roma2hira);
        if (obj->rx)
            rxgen_close(obj->rx);
        if (obj->mtree)
            mtree_close(obj->mtree);
        free(obj);
    }
}

// query version 2

static int
migemo_addword(migemo *object, unsigned char *word)
{
    return rxgen_add(object->rx, word);
}

static inline void
add_mnode_words(migemo *object, wordlist *list)
{
    for (; list; list = wordlist_next(list))
        migemo_addword(object, wordlist_word(list));
}

static void
add_mnode_siblings(migemo *object, mnode *pnode)
{
    add_mnode_words(object, pnode->list);
    if (pnode->child)
        add_mnode_siblings(object, pnode->child);
    if (pnode->low)
        add_mnode_siblings(object, pnode->low);
    if (pnode->high)
        add_mnode_siblings(object, pnode->high);
}

static void
add_mtree_query(migemo *object, unsigned char *query)
{
    mnode *pnode = mtree_query(object->mtree, query);
    if (pnode)
    {
        add_mnode_words(object, pnode->list);
        if (pnode->child)
            add_mnode_siblings(object, pnode->child);
    }
}

/// Convert input from Romaji to Kana and add it to the search keys.
static void
add_roma(migemo *object, unsigned char *query)
{
    wordlist *hira_list = romaji_convert_all(object->roma2hira, query);
    for (wordlist *hira_item = hira_list; hira_item;
            hira_item = wordlist_next(hira_item))
    {
        unsigned char *hira = wordlist_word(hira_item);
        migemo_addword(object, hira);
        add_mtree_query(object, hira);

        wordlist *kata_list = romaji_convert_all(object->hira2kata, hira);
        for (wordlist *kata_item = kata_list; kata_item;
                kata_item = wordlist_next(kata_item))
        {
            unsigned char *kata = wordlist_word(kata_item);
            migemo_addword(object, kata);
            add_mtree_query(object, kata);

            wordlist *han_list = romaji_convert_all(object->zen2han, kata);
            for (wordlist *han_item = han_list; han_item;
                    han_item = wordlist_next(han_item))
            {
                unsigned char *han = wordlist_word(han_item);
                migemo_addword(object, han);
            }
            wordlist_destroy(han_list);
        }
        wordlist_destroy(kata_list);
    }
    wordlist_destroy(hira_list);
}

/// Split the query into phrases.
///
/// Phrases are typically separated by uppercase letters. A phrase starting
/// with multiple uppercase letters is separated by non-uppercase characters.
static wordlist *
parse_query(migemo *object, const unsigned char *query)
{
    const unsigned char *curr = query;
    const unsigned char *start = NULL;
    wordlist *querylist = NULL, **pp = &querylist;

    while (1)
    {
        int len, upper;
        int sum = 0;

        if (!object->char2int || (len = object->char2int(curr, NULL)) < 1)
            len = 1;
        start = curr;
        upper = (len == 1 && isupper(*curr) && isupper(curr[1]));
        curr += len;
        sum += len;
        while (1)
        {
            if (!object->char2int || (len = object->char2int(curr, NULL)) < 1)
                len = 1;
            if (*curr == '\0' || (len == 1 && (isupper(*curr) != 0) != upper))
                break;
            curr += len;
            sum += len;
        }
        // Register a phrase
        if (start && start < curr)
        {
            *pp = wordlist_new(start, sum);
            pp = &(*pp)->next;
        }
        if (*curr == '\0')
            break;
    }
    return querylist;
}

// Convert a single word using migemo. Does not perform argument checking.
static void
query_a_word(migemo *object, unsigned char *query)
{
    unsigned char *lower = NULL;
    int len = my_strlen(query);

    // Naturally, add the query itself to the candidates
    migemo_addword(object, query);
    // Dictionary lookup with the query itself
    lower = malloc(len + 1);
    if (!lower)
        add_mtree_query(object, query);
    else
    {
        int i = 0, step;

        // Uppercase to lowercase conversion considering multi-byte characters
        while (i <= len)
        {
            if (!object->char2int
                    || (step = object->char2int(&query[i], NULL)) < 1)
                step = 1;
            if (step == 1 && isupper(query[i]))
                lower[i] = (unsigned char)tolower(query[i]);
            else
                memcpy(&lower[i], &query[i], step);
            i += step;
        }
        add_mtree_query(object, lower);
    }

    // Convert query to full-width and add to candidates
    wordlist *zen_list = romaji_convert_all(object->han2zen, query);
    for (wordlist *zen = zen_list; zen; zen = wordlist_next(zen))
        migemo_addword(object, wordlist_word(zen));
    wordlist_destroy(zen_list);

    // Convert query to half-width and add to candidates
    wordlist *han_list = romaji_convert_all(object->zen2han, query);
    for (wordlist *han = han_list; han; han = wordlist_next(han))
        migemo_addword(object, wordlist_word(han));
    wordlist_destroy(han_list);

    // Add Hiragana, Katakana, and dictionary lookups using them
    add_roma(object, lower);

    free(lower);
}

EXPORTS unsigned char *MIGEMO_CALLTYPE
migemo_query(migemo *object, const unsigned char *query)
{
    unsigned char *retval = NULL;
    wordlist *querylist = NULL;
    strbuf *outbuf = NULL;

    if (object && object->rx && query)
    {
        wordlist *p;

        querylist = parse_query(object, query);
        if (querylist == NULL)
            goto MIGEMO_QUERY_END; // Error due to empty query
        outbuf = strbuf_open();
        if (outbuf == NULL)
            goto MIGEMO_QUERY_END; // Error due to insufficient memory for
                                   // output

        // Input word groups into the rxgen object and obtain a regular
        // expression
        rxgen_reset(object->rx);
        for (p = querylist; p; p = wordlist_next(p))
        {
            unsigned char *answer;

            // printf("query=%s\n", wordlist_word(p));
            query_a_word(object, wordlist_word(p));
            // Generate search pattern (regular expression)
            answer = rxgen_generate(object->rx);
            rxgen_reset(object->rx);
            strbuf_append_str(outbuf, answer);
            rxgen_release(object->rx, answer);
        }
    }

MIGEMO_QUERY_END:
    if (outbuf)
    {
        retval = strbuf_get(outbuf);
        // Explicitly set to NULL to decouple it from `strbuf` management.
        outbuf->buf = NULL;
        strbuf_close(outbuf);
    }
    if (querylist)
        wordlist_destroy(querylist);

    return retval;
}

EXPORTS void MIGEMO_CALLTYPE
migemo_release(migemo *p, unsigned char *used_pattern)
{
    free(used_pattern);
}

/// Set regular expression metacharacters (operators) for the Migemo object.
///
/// Supported metacharacter identifiers (`index`):
///
// clang-format off
/// - `MIGEMO_OPINDEX_OR`: OR operator (default: "|").
/// - `MIGEMO_OPINDEX_NEST_IN`: Group opening bracket (default: "(").
/// - `MIGEMO_OPINDEX_NEST_OUT`: Group closing bracket (default: ")").
/// - `MIGEMO_OPINDEX_SELECT_IN`: Character class opening bracket (default: "[").
/// - `MIGEMO_OPINDEX_SELECT_OUT`: Character class closing bracket (default: "]").
/// - `MIGEMO_OPINDEX_NEWLINE`: Pattern inserted between characters for matching across lines (default: "").
// clang-format on
///
/// @param object Migemo object.
/// @param index Metacharacter identifier (`MIGEMO_OPINDEX_*`).
/// @param op Metacharacter string to set.
/// @return Non-zero on success, 0 on failure.
EXPORTS int MIGEMO_CALLTYPE
migemo_set_operator(migemo *object, int index, const unsigned char *op)
{
    if (object)
    {
        int retval = rxgen_set_operator(object->rx, index, op);
        return retval ? 0 : 1;
    }
    else
        return 0;
}

/// Retrieve the metacharacter (operator) string for the specified index.
///
/// For details about supported `index` values, see migemo_set_operator().
///
/// @param object Migemo object.
/// @param index Metacharacter identifier (`MIGEMO_OPINDEX_*`).
/// @return Pointer to the metacharacter string on success, or NULL if `object`
/// is NULL or `index` is invalid.
EXPORTS const unsigned char *MIGEMO_CALLTYPE
migemo_get_operator(migemo *object, int index)
{
    return object ? rxgen_get_operator(object->rx, index) : NULL;
}

/// Set a custom character conversion procedure (char -> int) for the Migemo
/// object.
///
/// @param object Migemo object.
/// @param proc Character conversion procedure.
EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_char2int(migemo *object, MIGEMO_PROC_CHAR2INT proc)
{
    if (object)
        rxgen_setproc_char2int(object->rx, (CHARSET_PROC_CHAR2INT)proc);
}

/// Set a custom character conversion procedure (int -> char) for the Migemo
/// object.
///
/// @param object Migemo object.
/// @param proc Character conversion procedure.
EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_int2char(migemo *object, MIGEMO_PROC_INT2CHAR proc)
{
    if (object)
        rxgen_setproc_int2char(object->rx, (CHARSET_PROC_INT2CHAR)proc);
}

/// Check whether the main dictionary is loaded and ready for queries.
///
/// @param obj Migemo object.
/// @return Non-zero if enabled, 0 otherwise.
EXPORTS int MIGEMO_CALLTYPE
migemo_is_enable(migemo *obj)
{
    return obj ? obj->enable : 0;
}
