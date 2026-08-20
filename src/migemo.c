// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// migemo.c -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
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

#define DICT_ROMA2HIRA "roma2hira.dat"
#define DICT_HIRA2KATA "hira2kata.dat"
#define DICT_HAN2ZEN   "han2zen.dat"
#define DICT_ZEN2HAN   "zen2han.dat"

#ifndef _MAX_PATH
# define _MAX_PATH 1024 // Placeholder value
#endif

static int
my_strlen(const char *s)
{
    size_t len;

    len = strlen(s);
    return len <= INT_MAX ? (int)len : INT_MAX;
}

static mtree *
load_mtree_dictionary(migemo *mo, const char *dict_file)
{
    if (mo->charset == CHARSET_NONE)
        mo->charset = charset_detect_file(dict_file);

    CHARSET_PROC_CHAR2INT char2int = NULL;
    CHARSET_PROC_INT2CHAR int2char = NULL;
    charset_getproc(mo->charset, &char2int, &int2char);
    migemo_setproc_char2int(mo, (MIGEMO_PROC_CHAR2INT)char2int);
    migemo_setproc_int2char(mo, (MIGEMO_PROC_INT2CHAR)int2char);
    mo->char2int =
            char2int && char2int != charset_utf8_char2int ? char2int : NULL;

    FILE *fp = fopen(dict_file, "rt");
    if (!fp)
        return NULL;
    mtree *mt = mtree_load(mo->mtree, fp, char2int);
    fclose(fp);
    return mt;
}

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
/// @param mo Migemo object.
/// @param dict_id Identifier specifying the type of dictionary/data.
/// @param dict_file Path to the dictionary or data file to load.
/// @return dict_id on success, 0 on failure.
int MIGEMO_CALLTYPE
migemo_load(migemo *mo, int dict_id, const char *dict_file)
{
    if (!mo && dict_file)
        return 0;

    // Load migemo dictionary
    if (dict_id == MIGEMO_DICTID_MIGEMO)
    {
        mtree *mt = load_mtree_dictionary(mo, dict_file);
        if (!mt)
            return 0;
        mo->mtree = mt;
        mo->enable = 1;
        return dict_id; // Loaded successfully
    }

    romaji *dict;
    switch (dict_id)
    {
        case MIGEMO_DICTID_ROMA2HIRA:
            // Load romaji dictionary
            dict = mo->roma2hira;
            break;
        case MIGEMO_DICTID_HIRA2KATA:
            // Load katakana dictionary
            dict = mo->hira2kata;
            break;
        case MIGEMO_DICTID_HAN2ZEN:
            // Load half-width to full-width dictionary
            dict = mo->han2zen;
            break;
        case MIGEMO_DICTID_ZEN2HAN:
            // Load half-width to full-width dictionary
            dict = mo->zen2han;
            break;
        default:
            return 0;
    }

    return romaji_load(dict, dict_file, mo->char2int) == 0 ? dict_id : 0;
}

migemo *MIGEMO_CALLTYPE
migemo_open(const char *dict)
{
    migemo *mo = (migemo *)calloc(1, sizeof(migemo));
    if (!mo)
        return NULL;
    // Construct the Migemo object and its members
    mo->enable = 0;
    mo->mtree = mtree_open();
    mo->charset = CHARSET_NONE;
    mo->rx = rxgen_open();
    mo->roma2hira = romaji_open();
    mo->hira2kata = romaji_open();
    mo->han2zen = romaji_open();
    mo->zen2han = romaji_open();
    mo->char2int = NULL;
    if (!mo->mtree || !mo->rx || !mo->roma2hira || !mo->hira2kata
            || !mo->han2zen || !mo->zen2han)
    {
        migemo_close(mo);
        return NULL;
    }

    // If a default migemo dictionary is specified, also look for romaji and
    // katakana dictionaries
    if (dict)
    {
        char dir[_MAX_PATH];
        char roma_dict[_MAX_PATH];
        char kata_dict[_MAX_PATH];
        char h2z_dict[_MAX_PATH];
        char z2h_dict[_MAX_PATH];

        filename_directory(dir, _MAX_PATH, dict);
        const char *tmp = strlen(dir) ? dir : ".";
        filename_join(roma_dict, _MAX_PATH, tmp, DICT_ROMA2HIRA);
        filename_join(kata_dict, _MAX_PATH, tmp, DICT_HIRA2KATA);
        filename_join(h2z_dict, _MAX_PATH, tmp, DICT_HAN2ZEN);
        filename_join(z2h_dict, _MAX_PATH, tmp, DICT_ZEN2HAN);

        mtree *mt = load_mtree_dictionary(mo, dict);
        if (mt)
        {
            mo->mtree = mt;
            mo->enable = 1;
            romaji_load(mo->roma2hira, roma_dict, mo->char2int);
            romaji_load(mo->hira2kata, kata_dict, mo->char2int);
            romaji_load(mo->han2zen, h2z_dict, mo->char2int);
            romaji_load(mo->zen2han, z2h_dict, mo->char2int);
        }
    }
    return mo;
}

void MIGEMO_CALLTYPE
migemo_close(migemo *mo)
{
    if (mo)
    {
        if (mo->zen2han)
            romaji_close(mo->zen2han);
        if (mo->han2zen)
            romaji_close(mo->han2zen);
        if (mo->hira2kata)
            romaji_close(mo->hira2kata);
        if (mo->roma2hira)
            romaji_close(mo->roma2hira);
        if (mo->rx)
            rxgen_close(mo->rx);
        if (mo->mtree)
            mtree_close(mo->mtree);
        free(mo);
    }
}

static int
migemo_add_word(migemo *mo, unsigned char *word)
{
    return rxgen_add(mo->rx, word);
}

static inline void
migemo_add_wordlist(migemo *mo, wordlist *list)
{
    for (; list; list = wordlist_next(list))
        migemo_add_word(mo, wordlist_word(list));
}

static void
migemo_add_mnode_siblings(migemo *mo, mnode *node)
{
    migemo_add_wordlist(mo, node->list);
    if (node->child)
        migemo_add_mnode_siblings(mo, node->child);
    if (node->low)
        migemo_add_mnode_siblings(mo, node->low);
    if (node->high)
        migemo_add_mnode_siblings(mo, node->high);
}

static void
migemo_add_mtree_matches(migemo *mo, unsigned char *query)
{
    mnode *node = mtree_query(mo->mtree, query);
    if (node)
    {
        migemo_add_wordlist(mo, node->list);
        if (node->child)
            migemo_add_mnode_siblings(mo, node->child);
    }
}

/// Convert input from Romaji to Kana and add it to the search keys.
static void
migemo_add_roma_variants(migemo *mo, unsigned char *query)
{
    wordlist *hira_list = romaji_convert_all(mo->roma2hira, query);
    for (wordlist *hira_item = hira_list; hira_item;
            hira_item = wordlist_next(hira_item))
    {
        unsigned char *hira = wordlist_word(hira_item);
        migemo_add_word(mo, hira);
        migemo_add_mtree_matches(mo, hira);

        wordlist *kata_list = romaji_convert_all(mo->hira2kata, hira);
        for (wordlist *kata_item = kata_list; kata_item;
                kata_item = wordlist_next(kata_item))
        {
            unsigned char *kata = wordlist_word(kata_item);
            migemo_add_word(mo, kata);
            migemo_add_mtree_matches(mo, kata);

            wordlist *han_list = romaji_convert_all(mo->zen2han, kata);
            for (wordlist *han_item = han_list; han_item;
                    han_item = wordlist_next(han_item))
            {
                unsigned char *han = wordlist_word(han_item);
                migemo_add_word(mo, han);
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
migemo_segment_query(migemo *mo, const unsigned char *query)
{
    wordlist *queries = NULL, **pnext = &queries;
    const unsigned char *r = query;
    for (bool loop = true; loop;)
    {
        const unsigned char *start = r;

        // Determine how to identify query boundaries.
        bool upper_mode = false;
        int first = charset_decode(mo->char2int, &r);
        if (first < 0x80 && isupper(first))
        {
            // Look ahead to the second character.
            const unsigned char *r2 = r;
            int second = charset_decode(mo->char2int, &r2);
            upper_mode = second < 0x80 && isupper(second);
        }

        // Locate the boundary where the case switches. In "upper mode," the
        // first lowercase letter marks the boundary; in "lower mode," the
        // first uppercase letter marks the boundary.
        const unsigned char *prev = NULL;
        while (1)
        {
            prev = r;
            int code = charset_decode(mo->char2int, &r);
            if (code == 0)
            {
                loop = false;
                break;
            }
            if (code < 0x80 && (isupper(code) != 0) != upper_mode)
                break;
        }

        // Register a phrase
        if (prev)
        {
            *pnext = wordlist_new(start, prev - start);
            pnext = &(*pnext)->next;
            r = prev;
        }
    }
    return queries;
}

// Convert a single word using migemo. Does not perform argument checking.
static void
migemo_process_word(migemo *mo, unsigned char *query)
{
    int len = my_strlen(query);
    unsigned char *lower = malloc(len + 1);
    if (!lower)
        return; // Error: memory allocation

    // Naturally, add the query itself to the candidates
    migemo_add_word(mo, query);

    // Convert the query string to lowercase.
    const unsigned char *r = query;
    unsigned char *w = lower;
    while (1)
    {
        const unsigned char *prev = r;
        int code = charset_decode(mo->char2int, &r);
        ptrdiff_t step = r - prev;
        if (step == 1 && code < 0x80 && isupper(code))
            *w = (unsigned char)tolower(code);
        else
            memcpy(w, prev, step);
        if (code == 0)
            break;
        w += step;
    }

    // Dictionary search using a lowercase query.
    migemo_add_mtree_matches(mo, lower);

    // Convert query to full-width and add to candidates
    wordlist *zen_list = romaji_convert_all(mo->han2zen, query);
    for (wordlist *zen = zen_list; zen; zen = wordlist_next(zen))
        migemo_add_word(mo, wordlist_word(zen));
    wordlist_destroy(zen_list);

    // Convert query to half-width and add to candidates
    wordlist *han_list = romaji_convert_all(mo->zen2han, query);
    for (wordlist *han = han_list; han; han = wordlist_next(han))
        migemo_add_word(mo, wordlist_word(han));
    wordlist_destroy(han_list);

    // Add Hiragana, Katakana, and dictionary lookups using them
    migemo_add_roma_variants(mo, lower);

    free(lower);
}

unsigned char *MIGEMO_CALLTYPE
migemo_query(migemo *mo, const unsigned char *query)
{
    if (!mo || !mo->rx || !query)
        return NULL;

    wordlist *queries = migemo_segment_query(mo, query);
    if (queries == NULL)
    {
        // Error: no queries
        return NULL;
    }
    strbuf *patbuf = strbuf_open();
    if (patbuf == NULL)
    {
        // Error: insufficient memory for output
        wordlist_destroy(queries);
        return NULL;
    }

    rxgen_reset(mo->rx);
    for (wordlist *wl = queries; wl; wl = wordlist_next(wl))
    {
        migemo_process_word(mo, wordlist_word(wl));
        unsigned char *subpat = rxgen_generate(mo->rx);
        rxgen_reset(mo->rx);
        strbuf_append_str(patbuf, subpat);
        rxgen_release(mo->rx, subpat);
    }

    unsigned char *pattern = strbuf_get(patbuf);
    // Explicitly set to NULL to decouple it from `strbuf` management.
    patbuf->buf = NULL;
    strbuf_close(patbuf);
    wordlist_destroy(queries);

    return pattern;
}

void MIGEMO_CALLTYPE
migemo_release(migemo *mo, unsigned char *used_pattern)
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
/// @param mo Migemo object.
/// @param index Metacharacter identifier (`MIGEMO_OPINDEX_*`).
/// @param op Metacharacter string to set.
/// @return 1 on success, 0 on failure.
int MIGEMO_CALLTYPE
migemo_set_operator(migemo *mo, int index, const unsigned char *op)
{
    if (!mo)
        return 0;
    return rxgen_set_operator(mo->rx, index, op) == 0 ? 1 : 0;
}

/// Retrieve the metacharacter (operator) string for the specified index.
///
/// For details about supported `index` values, see migemo_set_operator().
///
/// @param mo Migemo object.
/// @param index Metacharacter identifier (`MIGEMO_OPINDEX_*`).
/// @return Pointer to the metacharacter string on success, or NULL if `mo`
/// is NULL or `index` is invalid.
const unsigned char *MIGEMO_CALLTYPE
migemo_get_operator(migemo *mo, int index)
{
    return mo ? rxgen_get_operator(mo->rx, index) : NULL;
}

/// Set custom characters to escape in generated regular expressions.
///
/// @param mo Migemo object.
/// @param chars String of characters to escape (ASCII 32-126). If NULL, default
/// set ("\\.*+^$/") is used. If "", no characters are escaped.
void MIGEMO_CALLTYPE
migemo_set_escape_chars(migemo *mo, const unsigned char *chars)
{
    if (mo)
        rxgen_set_escape_chars(mo->rx, chars);
}

/// Set a custom character conversion procedure (char -> int) for the Migemo
/// object.
///
/// @param mo Migemo object.
/// @param proc Character conversion procedure.
void MIGEMO_CALLTYPE
migemo_setproc_char2int(migemo *mo, MIGEMO_PROC_CHAR2INT proc)
{
    if (mo)
        rxgen_setproc_char2int(mo->rx, (CHARSET_PROC_CHAR2INT)proc);
}

/// Set a custom character conversion procedure (int -> char) for the Migemo
/// object.
///
/// @param mo Migemo object.
/// @param proc Character conversion procedure.
void MIGEMO_CALLTYPE
migemo_setproc_int2char(migemo *mo, MIGEMO_PROC_INT2CHAR proc)
{
    if (mo)
        rxgen_setproc_int2char(mo->rx, (CHARSET_PROC_INT2CHAR)proc);
}

/// Check whether the main dictionary is loaded and ready for queries.
///
/// @param mo Migemo object.
/// @return Non-zero if enabled, 0 otherwise.
int MIGEMO_CALLTYPE
migemo_is_enable(migemo *mo)
{
    return mo ? mo->enable : 0;
}

/// Retrieve the version string of the C/Migemo library.
///
/// @return Static version string (e.g., "1.6.1").
const char *MIGEMO_CALLTYPE
migemo_version(void)
{
    return MIGEMO_VERSION MIGEMO_VERSION_PRERELEASE;
}
