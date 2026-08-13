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
#include "mnode.h"
#include "romaji.h"
#include "rxgen.h"
#include "wordbuf.h"
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
    mtree *mt = mnode_load(obj->mtree, fp, char2int, int2char);
    fclose(fp);
    return mt;
}

// migemo interfaces

/// Add a dictionary or a data file to the Migemo object.
/// dict_file specifies the file name to load. dict_id specifies the type of
/// dictionary/data to load:
///
/// <dl>
/// <dt>MIGEMO_DICTID_MIGEMO</dt>
/// <dd>migemo-dict dictionary</dd>
/// <dt>MIGEMO_DICTID_ROMA2HIRA</dt>
/// <dd>Romaji to Hiragana conversion table</dd>
/// <dt>MIGEMO_DICTID_HIRA2KATA</dt>
/// <dd>Hiragana to Katakana conversion table</dd>
/// <dt>MIGEMO_DICTID_HAN2ZEN</dt>
/// <dd>Half-width to Full-width conversion table</dd>
/// <dt>MIGEMO_DICTID_ZEN2HAN</dt>
/// <dd>Full-width to Half-width conversion table</dd>
/// </dl>
///
/// The return value indicates the type actually loaded, or it may indicate that
/// loading failed by returning the following value:
///
/// <dl><dt>MIGEMO_DICTID_INVALID</dt></dl>
/// @param obj Migemo object
/// @param dict_id Type of dictionary file
/// @param dict_file Path to the dictionary file
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

/// Create a Migemo object. If successful, the object is returned as the return
/// value, and if it fails, NULL is returned. The file specified in dict is
/// loaded as the migemo-dict dictionary during object creation. If the
/// following files exist in the same directory as the dictionary:
///
/// <dl>
/// <dt>roma2hira.dat</dt>
/// <dd>Romaji to Hiragana conversion table</dd>
/// <dt>hira2kata.dat</dt>
/// <dd>Hiragana to Katakana conversion table</dd>
/// <dt>han2zen.dat</dt>
/// <dd>Half-width to full-width conversion table</dd>
/// </dl>
///
/// only the files that exist will be loaded. If NULL is specified for dict,
/// no files, including the dictionary, will be loaded.
/// Files can be additionally loaded after object creation using the
/// migemo_load() function.
///
/// @param dict Path to the migemo-dict dictionary. If NULL, no dictionary is
/// loaded.
/// @returns The created Migemo object
EXPORTS migemo *MIGEMO_CALLTYPE
migemo_open(const char *dict)
{
    migemo *obj;

    // Construct the Migemo object and its members
    if (!(obj = (migemo *)calloc(1, sizeof(migemo))))
        return obj;
    obj->enable = 0;
    obj->mtree = mnode_open();
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

/// Destroy the Migemo object and release used resources.
/// @param obj Migemo object to destroy
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
            mnode_close(obj->mtree);
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
    for (; list; list = list->next)
        migemo_addword(object, list->ptr);
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
add_mnode_query(migemo *object, unsigned char *query)
{
    mnode *pnode = mnode_query(object->mtree, query);
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
            hira_item = hira_item->next)
    {
        unsigned char *hira = hira_item->ptr;
        migemo_addword(object, hira);
        add_mnode_query(object, hira);

        wordlist *kata_list = romaji_convert_all(object->hira2kata, hira);
        for (wordlist *kata_item = kata_list; kata_item;
                kata_item = kata_item->next)
        {
            unsigned char *kata = kata_item->ptr;
            migemo_addword(object, kata);
            add_mnode_query(object, kata);

            wordlist *han_list = romaji_convert_all(object->zen2han, kata);
            for (wordlist *han_item = han_list; han_item;
                    han_item = han_item->next)
            {
                unsigned char *han = han_item->ptr;
                migemo_addword(object, han);
            }
            wordlist_destroy(han_list);
        }
        wordlist_destroy(kata_list);
    }
    wordlist_destroy(hira_list);
}

/// Split the query into phrases. Phrases are typically separated by uppercase
/// letters. A phrase starting with multiple uppercase letters is separated by
/// non-uppercase characters.
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
        add_mnode_query(object, query);
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
        add_mnode_query(object, lower);
    }

    // Convert query to full-width and add to candidates
    wordlist *zen_list = romaji_convert_all(object->han2zen, query);
    for (wordlist *zen = zen_list; zen; zen = zen->next)
        migemo_addword(object, zen->ptr);
    wordlist_destroy(zen_list);

    // Convert query to half-width and add to candidates
    wordlist *han_list = romaji_convert_all(object->zen2han, query);
    for (wordlist *han = han_list; han; han = han->next)
        migemo_addword(object, han->ptr);
    wordlist_destroy(han_list);

    // Add Hiragana, Katakana, and dictionary lookups using them
    add_roma(object, lower);

    free(lower);
}

/// Converts the given string (Romaji) into a regular expression for Japanese
/// search. The return value is the converted result (regular expression), which
/// must be released using the #migemo_release() function.
/// @param object Migemo object
/// @param query Query string
/// @returns Regular expression string. Must be released with #migemo_release().
EXPORTS unsigned char *MIGEMO_CALLTYPE
migemo_query(migemo *object, const unsigned char *query)
{
    unsigned char *retval = NULL;
    wordlist *querylist = NULL;
    wordbuf *outbuf = NULL;

    if (object && object->rx && query)
    {
        wordlist *p;

        querylist = parse_query(object, query);
        if (querylist == NULL)
            goto MIGEMO_QUERY_END; // Error due to empty query
        outbuf = wordbuf_open();
        if (outbuf == NULL)
            goto MIGEMO_QUERY_END; // Error due to insufficient memory for
                                   // output

        // Input word groups into the rxgen object and obtain a regular
        // expression
        rxgen_reset(object->rx);
        for (p = querylist; p; p = p->next)
        {
            unsigned char *answer;

            // printf("query=%s\n", p->ptr);
            query_a_word(object, p->ptr);
            // Generate search pattern (regular expression)
            answer = rxgen_generate(object->rx);
            rxgen_reset(object->rx);
            wordbuf_cat(outbuf, answer);
            rxgen_release(object->rx, answer);
        }
    }

MIGEMO_QUERY_END:
    if (outbuf)
    {
        retval = outbuf->buf;
        outbuf->buf = NULL;
        wordbuf_close(outbuf);
    }
    if (querylist)
        wordlist_destroy(querylist);

    return retval;
}

/// Frees the regular expression obtained with the migemo_query() function after
/// use.
/// @param p Migemo object
/// @param string Regular expression string
EXPORTS void MIGEMO_CALLTYPE
migemo_release(migemo *p, unsigned char *string)
{
    free(string);
}

/// Specifies the metacharacters (operators) used in the regular expression
/// generated by the Migemo object. The index parameter specifies which
/// metacharacter to replace with op. The following values can be specified:
///
/// <dl>
/// <dt>MIGEMO_OPINDEX_OR</dt>
/// <dd>Logical OR. Default is "|". When using in vim, use "\|".</dd>
/// <dt>MIGEMO_OPINDEX_NEST_IN</dt>
/// <dd>Opening parenthesis used for grouping. Default is "(". In vim, use "\%("
/// to avoid saving it in registers \\1 to \\9. In Perl, you can use "(?:" to
/// achieve the same thing.</dd>
/// <dt>MIGEMO_OPINDEX_NEST_OUT</dt>
/// <dd>Closing parenthesis representing the end of a group. Default is ")". In
/// vim, use "\)".</dd>
/// <dt>MIGEMO_OPINDEX_SELECT_IN</dt>
/// <dd>Opening square bracket representing the start of a selection. Default
/// is "[".</dd>
/// <dt>MIGEMO_OPINDEX_SELECT_OUT</dt>
/// <dd>Closing square bracket representing the end of a selection. Default is
/// "]".</dd>
/// <dt>MIGEMO_OPINDEX_NEWLINE</dt>
/// <dd>A pattern that matches zero or more whitespace or newline characters,
/// inserted between each character.  Default is "", meaning no pattern is set.
/// In vim, it refers to "\_s*".</dd>
/// </dl>
///
/// The default metacharacters have the same meaning as Perl's unless otherwise
/// specified. A successful operation returns non-zero (1), and a failure
/// returns 0.
/// @param object Migemo object
/// @param index Metacharacter identifier
/// @param op Metacharacter string
/// @returns Non-zero on success, 0 on failure.
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

/// Retrieves the metacharacters (operators) used in the regular expression
/// generated by the Migemo object. For details about index, see the
/// migemo_set_operator() function. If the index is valid, a pointer to the
/// string containing the metacharacter is returned; otherwise, NULL is
/// returned.
/// @param object Migemo object
/// @param index Metacharacter identifier
/// @returns Current metacharacter string
EXPORTS const unsigned char *MIGEMO_CALLTYPE
migemo_get_operator(migemo *object, int index)
{
    return object ? rxgen_get_operator(object->rx, index) : NULL;
}

/// Sets a procedure for code conversion in the Migemo object. For details about
/// the procedure, see MIGEMO_PROC_CHAR2INT in the "Type Reference" section.
/// @param object Migemo object
/// @param proc Code conversion procedure
EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_char2int(migemo *object, MIGEMO_PROC_CHAR2INT proc)
{
    if (object)
        rxgen_setproc_char2int(object->rx, (RXGEN_PROC_CHAR2INT)proc);
}

/// Sets a procedure for code conversion in the Migemo object. For details about
/// the procedure, see MIGEMO_PROC_INT2CHAR in the "Type Reference" section.
/// @param object Migemo object
/// @param proc Code conversion procedure
EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_int2char(migemo *object, MIGEMO_PROC_INT2CHAR proc)
{
    if (object)
        rxgen_setproc_int2char(object->rx, (RXGEN_PROC_INT2CHAR)proc);
}

/// Checks whether the migemo_dict has been loaded into the Migemo object.
/// Returns non-zero (TRUE) if a valid migemo_dict is loaded and conversion
/// tables are built, and 0 (FALSE) otherwise.
/// @param obj Migemo object
/// @returns Non-zero on success, 0 on failure.
EXPORTS int MIGEMO_CALLTYPE
migemo_is_enable(migemo *obj)
{
    return obj ? obj->enable : 0;
}

#if 1
// Primarily a hidden function for debugging purposes
EXPORTS void MIGEMO_CALLTYPE
migemo_print(migemo *object)
{
    if (object)
        mnode_print(object->mtree, NULL);
}
#endif
