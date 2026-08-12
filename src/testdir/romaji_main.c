// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji_main.c - Romaji convert console.
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>
//
// gcc -o romaji romaji_main.c ../romaji.c ../wordbuf.c

#include <stdio.h>
#include <string.h>

#include "romaji.h"

#ifndef DICTDIR
# define DICTDIR "../../dict"
#endif
#ifndef DICT_ROMA2HIRA
# define DICT_ROMA2HIRA (DICTDIR "/roma2hira.dat")
#endif
#ifndef DICT_HIRA2KATA
# define DICT_HIRA2KATA (DICTDIR "/hira2kata.dat")
#endif
#ifndef DICT_HAN2ZEN
# define DICT_HAN2ZEN (DICTDIR "/han2zen.dat")
#endif
#ifndef DICT_ZEN2HAN
# define DICT_ZEN2HAN (DICTDIR "/zen2han.dat")
#endif

void
query_one(romaji *object, romaji *hira2kata, romaji *han2zen, romaji *zen2han,
        char *buf)
{
    wordlist *hira = romaji_convert_all(object, buf);
    for (wordlist *p = hira; p ; p = p->next)
    {
        printf("  hira=%s\n", p->ptr);
        wordlist *kata = romaji_convert_all(hira2kata, p->ptr);
        for (wordlist *q = kata; q ; q = q->next)
        {
            printf("  kata=%s\n", q->ptr);
            wordlist *han = romaji_convert_all(zen2han, q->ptr);
            for (wordlist *r = han; r ; r = r->next)
                printf("  han=%s\n", r->ptr);
            wordlist_destroy(han);
        }
        wordlist_destroy(kata);
    }
    wordlist_destroy(hira);

    wordlist *zen = romaji_convert_all(han2zen, buf);
    for (wordlist *p = zen; p; p = p->next)
        printf("  zen=%s\n", p->ptr);
    wordlist_destroy(zen);
}

void
query_loop(romaji *object, romaji *hira2kata, romaji *han2zen, romaji *zen2han)
{
    char buf[256], *ans;

    while (1)
    {
        printf("QUERY: ");
        if (!fgets(buf, sizeof(buf), stdin))
        {
            printf("\n");
            break;
        }
        // Replace newline with NUL character
        if ((ans = strchr(buf, '\n')) != NULL)
            *ans = '\0';
        query_one(object, hira2kata, han2zen, zen2han, buf);
    }
}

int
main(int argc, char **argv)
{
    romaji *object, *hira2kata, *han2zen, *zen2han;
    char *word = NULL;

    object = romaji_open();
    hira2kata = romaji_open();
    han2zen = romaji_open();
    zen2han = romaji_open();
    romaji_set_verbose(zen2han, 1);

    while (*++argv)
    {
        if (0)
            ;
        else if (argv[1] && (!strcmp("--word", *argv) || !strcmp("-w", *argv)))
            word = *++argv;
    }

    if (object && hira2kata && han2zen && zen2han)
    {
        int retval = 0;

        retval = romaji_load(object, DICT_ROMA2HIRA, charset_utf8_char2int);
        printf("romaji_load(%s)=%d\n", DICT_ROMA2HIRA, retval);
        retval = romaji_load(hira2kata, DICT_HIRA2KATA, charset_utf8_char2int);
        printf("romaji_load(%s)=%d\n", DICT_HIRA2KATA, retval);
        retval = romaji_load(han2zen, DICT_HAN2ZEN, charset_utf8_char2int);
        printf("romaji_load(%s)=%d\n", DICT_HAN2ZEN, retval);
        retval = romaji_load(zen2han, DICT_ZEN2HAN, charset_utf8_char2int);
        printf("romaji_load(%s)=%d\n", DICT_HAN2ZEN, retval);
        if (word)
            query_one(object, hira2kata, han2zen, zen2han, word);
        else
            query_loop(object, hira2kata, han2zen, zen2han);
    }

    if (han2zen)
        romaji_close(han2zen);
    if (hira2kata)
        romaji_close(hira2kata);
    if (object)
        romaji_close(object);

    return 0;
}
