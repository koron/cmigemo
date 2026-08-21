// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji_main.c - Romaji convert console.
//
// Author:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "romaji.h"

#define ABOUT "romaji - Romaji converter"

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
query_one(romaji *rj, romaji *hira2kata, romaji *han2zen, romaji *zen2han,
        char *buf)
{
    wordlist *hira = romaji_convert_all(rj, buf);
    for (wordlist *p = hira; p; p = wordlist_next(p))
    {
        printf("  hira=%s\n", wordlist_word(p));
        wordlist *kata = romaji_convert_all(hira2kata, wordlist_word(p));
        for (wordlist *q = kata; q; q = wordlist_next(q))
        {
            printf("  kata=%s\n", wordlist_word(q));
            wordlist *han = romaji_convert_all(zen2han, wordlist_word(q));
            for (wordlist *r = han; r; r = wordlist_next(r))
                printf("  han=%s\n", wordlist_word(r));
            wordlist_destroy(han);
        }
        wordlist_destroy(kata);
    }
    wordlist_destroy(hira);

    wordlist *zen = romaji_convert_all(han2zen, buf);
    for (wordlist *p = zen; p; p = wordlist_next(p))
        printf("  zen=%s\n", wordlist_word(p));
    wordlist_destroy(zen);
}

void
query_loop(romaji *rj, romaji *hira2kata, romaji *han2zen, romaji *zen2han)
{
    unsigned char buf[256];
    while (!feof(stdin))
    {
        printf("QUERY: ");
        if (!fgets(buf, sizeof(buf), stdin))
        {
            printf("\n");
            break;
        }
        // Replace LF with '\0'. If no LF is present, set the end of the buffer
        // to '\0' to ensure the string is null-terminated.
        int i = 0;
        while (i < sizeof(buf) - 1 && buf[i] != '\n' && buf[i] != '\0')
            i++;
        buf[i] = '\0';

        query_one(rj, hira2kata, han2zen, zen2han, buf);
    }
}

static void
help(char *prgname)
{
    printf("\
%s \n\
\n\
USAGE: %s [OPTIONS]\n\
\n\
OPTIONS:\n\
  -w --word <word>      Expand a <word> and soon exit.\n\
  -h --help             Show this message.\n\
",
            ABOUT, prgname);
    exit(0);
}

int
main(int argc, char **argv)
{
    char *word = NULL;
    char *prgname = argv[0];

    romaji *rj = romaji_open();
    romaji *hira2kata = romaji_open();
    romaji *han2zen = romaji_open();
    romaji *zen2han = romaji_open();

    if (!rj || !hira2kata || !han2zen || !zen2han)
    {
        printf("failed to allocate memory for romaji\n");
        return 1;
    }

    while (*++argv)
    {
        if (0)
            ;
        else if (argv[1] && (!strcmp("--word", *argv) || !strcmp("-w", *argv)))
            word = *++argv;
        else if (!strcmp("--help", *argv) || !strcmp("-h", *argv))
            help(prgname);
    }

    // Load romaji dictionaries.
    int retval = 0;
    retval = romaji_load(rj, DICT_ROMA2HIRA, charset_utf8_char2int);
    printf("romaji_load(%s)=%d\n", DICT_ROMA2HIRA, retval);
    retval = romaji_load(hira2kata, DICT_HIRA2KATA, charset_utf8_char2int);
    printf("romaji_load(%s)=%d\n", DICT_HIRA2KATA, retval);
    retval = romaji_load(han2zen, DICT_HAN2ZEN, charset_utf8_char2int);
    printf("romaji_load(%s)=%d\n", DICT_HAN2ZEN, retval);
    retval = romaji_load(zen2han, DICT_ZEN2HAN, charset_utf8_char2int);
    printf("romaji_load(%s)=%d\n", DICT_ZEN2HAN, retval);

    if (word)
        query_one(rj, hira2kata, han2zen, zen2han, word);
    else
        query_loop(rj, hira2kata, han2zen, zen2han);

    if (zen2han)
        romaji_close(zen2han);
    if (han2zen)
        romaji_close(han2zen);
    if (hira2kata)
        romaji_close(hira2kata);
    if (rj)
        romaji_close(rj);

    return 0;
}
