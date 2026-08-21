// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// main.c - Migemo library test driver
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "migemo.h"

#define MIGEMO_ABOUT "cmigemo - C/Migemo Library " MIGEMO_VERSION " Driver"
#define MIGEMO_SUBDICT_MAX 8

#ifndef S_ISREG
# define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#endif

// main

int
query_loop(migemo *mo, int quiet)
{
    while (!feof(stdin))
    {
        unsigned char buf[256], *ans;

        if (!quiet)
            printf("QUERY: ");
        // Changed from gets() to fgets()
        if (!fgets(buf, sizeof(buf), stdin))
        {
            if (!quiet)
                printf("\n");
            break;
        }
        // Replace newline with NULL character
        if ((ans = strchr(buf, '\n')) != NULL)
            *ans = '\0';

        ans = migemo_query(mo, buf);
        if (ans)
            printf(quiet ? "%s\n" : "PATTERN: %s\n", ans);
        fflush(stdout);
        migemo_release(mo, ans);
    }
    return 0;
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
  -d --dict <dict>      Use a file <dict> for dictionary.\n\
  -s --subdict <dict>   Sub dictionary files. (MAX %d times)\n\
  -u --userdict <dict>  Alias for -s/--subdict\n\
  -q --quiet            Show no message except results.\n\
  -v --vim              Use vim style regexp.\n\
  -e --emacs            Use emacs style regexp.\n\
  -n --nonewline        Don't use newline match.\n\
  -w --word <word>      Expand a <word> and soon exit.\n\
  -h --help             Show this message.\n\
",
            MIGEMO_ABOUT, prgname, MIGEMO_SUBDICT_MAX);
    exit(0);
}

static migemo *
open_first_migemo(const char **found, const char **dicts)
{
    for (; dicts != NULL && *dicts != NULL; dicts++)
    {
        struct stat st;
        const char *dict = *dicts;
        if (stat(dict, &st) != 0 || !S_ISREG(st.st_mode))
            continue;
        migemo *mo = migemo_open(dict);
        if (mo == NULL)
            continue;
        *found = *dicts;
        return mo;
    }
    return NULL;
}

static const char *default_dicts[] = {
#if _WIN32
        "./dict/cp932/" MIGEMO_DICT_FILENAME,
        "../dict/cp932/" MIGEMO_DICT_FILENAME,
        "./build/dict/cp932/" MIGEMO_DICT_FILENAME,
#else
        "./dict/utf-8/" MIGEMO_DICT_FILENAME,
        "../dict/utf-8/" MIGEMO_DICT_FILENAME,
        "./build/dict/utf-8/" MIGEMO_DICT_FILENAME,
#endif
        "./dict/" MIGEMO_DICT_FILENAME,
        "../dict/" MIGEMO_DICT_FILENAME,
        "./build/dict/" MIGEMO_DICT_FILENAME,
#ifdef CMIGEMO_DICTDIR
        CMIGEMO_DICTDIR "/" MIGEMO_DICT_FILENAME,
#endif
        NULL,
};

int
main(int argc, char **argv)
{
    int mode_vim = 0;
    int mode_emacs = 0;
    int mode_nonewline = 0;
    int mode_quiet = 0;
    char *dict = NULL;
    char *subdict[MIGEMO_SUBDICT_MAX];
    int subdict_count = 0;
    migemo *mo;
    FILE *fplog = stdout;
    char *word = NULL;
    char *prgname = argv[0];

    memset(subdict, 0, sizeof(subdict));
    while (*++argv)
    {
        if (0)
            ;
        else if (!strcmp("--vim", *argv) || !strcmp("-v", *argv))
            mode_vim = 1;
        else if (!strcmp("--emacs", *argv) || !strcmp("-e", *argv))
            mode_emacs = 1;
        else if (!strcmp("--nonewline", *argv) || !strcmp("-n", *argv))
            mode_nonewline = 1;
        else if (argv[1] && (!strcmp("--dict", *argv) || !strcmp("-d", *argv)))
            dict = *++argv;
        else if (argv[1]
                 && (!strcmp("--subdict", *argv) || !strcmp("-s", *argv)
                         || !strcmp("--userdict", *argv)
                         || !strcmp("-u", *argv))
                 && subdict_count < MIGEMO_SUBDICT_MAX)
            subdict[subdict_count++] = *++argv;
        else if (argv[1] && (!strcmp("--word", *argv) || !strcmp("-w", *argv)))
            word = *++argv;
        else if (!strcmp("--quiet", *argv) || !strcmp("-q", *argv))
            mode_quiet = 1;
        else if (!strcmp("--help", *argv) || !strcmp("-h", *argv))
            help(prgname);
    }

#ifdef _PROFILE
    fplog = fopen("exe.log", "wt");
#endif

    // Search for dictionaries in the current directory and the parent directory
    if (!dict)
    {
        const char *found = NULL;
        mo = open_first_migemo(&found, default_dicts);
        if (!word && !mode_quiet)
            fprintf(fplog, "migemo_open(\"%s\")=%p\n", found ? found : "(N/A)",
                    mo);
    }
    else
    {
        mo = migemo_open(dict);
        if (!word && !mode_quiet)
            fprintf(fplog, "migemo_open(\"%s\")=%p\n", dict, mo);
    }
    // Load sub-dictionaries
    if (subdict_count > 0)
    {
        int i;

        for (i = 0; i < subdict_count; ++i)
        {
            int result;

            if (subdict[i] == NULL || subdict[i][0] == '\0')
                continue;
            result = migemo_load(mo, MIGEMO_DICTID_MIGEMO, subdict[i]);
            if (!word && !mode_quiet)
                fprintf(fplog, "migemo_load(%p, %d, \"%s\")=%d\n", mo,
                        MIGEMO_DICTID_MIGEMO, subdict[i], result);
        }
    }

    if (!mo)
        return 1;
    else
    {
        if (mode_vim)
        {
            migemo_set_operator(mo, MIGEMO_OPINDEX_OR, "\\|");
            migemo_set_operator(mo, MIGEMO_OPINDEX_NEST_IN, "\\%(");
            migemo_set_operator(mo, MIGEMO_OPINDEX_NEST_OUT, "\\)");
            if (!mode_nonewline)
                migemo_set_operator(mo, MIGEMO_OPINDEX_NEWLINE, "\\_s*");
        }
        else if (mode_emacs)
        {
            migemo_set_operator(mo, MIGEMO_OPINDEX_OR, "\\|");
            migemo_set_operator(mo, MIGEMO_OPINDEX_NEST_IN, "\\(");
            migemo_set_operator(mo, MIGEMO_OPINDEX_NEST_OUT, "\\)");
            if (!mode_nonewline)
                migemo_set_operator(mo, MIGEMO_OPINDEX_NEWLINE, "\\s-*");
        }
#ifndef _PROFILE
        if (word)
        {
            unsigned char *ans;

            ans = migemo_query(mo, word);
            if (ans)
                fprintf(fplog, mode_vim ? "%s" : "%s\n", ans);
            migemo_release(mo, ans);
        }
        else
        {
            if (!mode_quiet)
                printf("clock()=%f\n", (float)clock() / CLOCKS_PER_SEC);
            query_loop(mo, mode_quiet);
        }
#else
        // For profiling
        {
            unsigned char *ans;

            ans = migemo_query(mo, "a");
            if (ans)
                fprintf(fplog, "  [%s]\n", ans);
            migemo_release(mo, ans);

            ans = migemo_query(mo, "k");
            if (ans)
                fprintf(fplog, "  [%s]\n", ans);
            migemo_release(mo, ans);
        }
#endif
        migemo_close(mo);
    }

    if (fplog != stdout)
        fclose(fplog);
    return 0;
}
