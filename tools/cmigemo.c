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

static int
is_exist_regular(const char *file)
{
    struct stat st;
    return stat(file, &st) == 0 && S_ISREG(st.st_mode);
}

// main

int
query_loop(migemo *mo, int quiet)
{
    unsigned char buf[256];
    while (!feof(stdin))
    {
        if (!quiet)
            printf("QUERY: ");
        // Changed from gets() to fgets()
        if (!fgets(buf, sizeof(buf), stdin))
        {
            if (!quiet)
                printf("\n");
            break;
        }
        // Replace LF with '\0'. If no LF is present, set the end of the buffer
        // to '\0' to ensure the string is null-terminated.
        int i = 0;
        while (i < sizeof(buf) - 1 && buf[i] != '\n' && buf[i] != '\0')
            i++;
        buf[i] = '\0';

        unsigned char *ans = migemo_query(mo, buf);
        if (ans)
            printf(quiet ? "%s\n" : "PATTERN: %s\n", ans);
        fflush(stdout);
        migemo_release(mo, ans);
    }
    return 0;
}

static void
help(const char *prgname, int exitcode)
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
     --convert          Convert the dictionary to static (sdict)\n\
                        and keep using it.\n\
     --save <dict>      Save converted static dictionary (sdict)\n\
                        (imply --convert)\n\
     --sdict <dict>     Load <dict> as static dictionary (sdict)\n\
                        (conflict with -d/s/u, and --convert/save)\n\
  -q --quiet            Show no message except results.\n\
  -v --vim              Use vim style regexp.\n\
  -e --emacs            Use emacs style regexp.\n\
  -n --nonewline        Don't use newline match.\n\
  -w --word <word>      Expand a <word> and soon exit.\n\
  -h --help             Show this message.\n\
",
            MIGEMO_ABOUT, prgname, MIGEMO_SUBDICT_MAX);
    exit(exitcode);
}

static void
unknown_option(const char *prgname, char *arg)
{
    fprintf(stderr, "ERROR: unknown option: %s\n", arg);
    help(prgname, 1);
}

static void
missing_argument(const char *prgname, char *arg)
{
    fprintf(stderr, "ERROR: missing argument: %s\n", arg);
    help(prgname, 1);
}

static void
assure_file(const char *file)
{
    if (is_exist_regular(file))
        return;
    fprintf(stderr, "ERROR: can't find file: %s\n", file);
    exit(1);
}

static migemo *
open_first_migemo(const char **found, const char **dicts)
{
    for (; dicts != NULL && *dicts != NULL; dicts++)
    {
        if (is_exist_regular(*dicts) == 0)
            continue;
        migemo *mo = migemo_open(*dicts);
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
    const char *prgname = argv[0];

    int mode_vim = 0;
    int mode_emacs = 0;
    int mode_nonewline = 0;
    int mode_quiet = 0;
    char *dict = NULL;
    char *subdict[MIGEMO_SUBDICT_MAX] = {0};
    int subdict_count = 0;
    int convert = 0;
    char *save_sdict = NULL;
    char *sdict = NULL;
    migemo *mo;
    char *word = NULL;

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
        else if (!strcmp("--dict", *argv) || !strcmp("-d", *argv))
        {
            if (!argv[1])
                missing_argument(prgname, *argv);
            dict = *++argv;
        }
        else if (!strcmp("--subdict", *argv) || !strcmp("-s", *argv)
                 || !strcmp("--userdict", *argv) || !strcmp("-u", *argv))
        {
            if (!argv[1])
                missing_argument(prgname, *argv);
            if (subdict_count >= MIGEMO_SUBDICT_MAX)
            {
                fprintf(stderr, "ERROR: too many --subdict/userdict\n");
                help(prgname, 1);
            }
            subdict[subdict_count++] = *++argv;
        }
        else if (!strcmp("--convert", *argv))
            convert = 1;
        else if (!strcmp("--save", *argv))
        {
            if (!argv[1])
                missing_argument(prgname, *argv);
            convert = 1;
            save_sdict = *++argv;
        }
        else if (!strcmp("--sdict", *argv))
        {
            if (!argv[1])
                missing_argument(prgname, *argv);
            sdict = *++argv;
        }
        else if ((!strcmp("--word", *argv) || !strcmp("-w", *argv)))
        {
            if (!argv[1])
                missing_argument(prgname, *argv);
            word = *++argv;
        }
        else if (!strcmp("--quiet", *argv) || !strcmp("-q", *argv))
            mode_quiet = 1;
        else if (!strcmp("--help", *argv) || !strcmp("-h", *argv))
            help(prgname, 0);
        else if (**argv == '-')
            unknown_option(prgname, *argv);
    }

    // Check dependencies between options.
    if (sdict)
    {
        if (dict || subdict_count > 0 || convert || save_sdict)
        {
            fprintf(stderr, "ERROR: --sdict conflicts with -d/s/u and "
                            "--convert/save\n");
            help(prgname, 1);
        }
    }

    // Load the main migemo dictionary
    if (sdict)
    {
        assure_file(sdict);
        mo = migemo_open_sdict(sdict);
        if (!word && !mode_quiet)
            printf("migemo_open_sdict(\"%s\")=%p\n", sdict, mo);
        if (!mo)
        {
            fprintf(stderr, "ERROR: failed to load static dictionary: %s\n",
                    sdict);
            return 1;
        }
    }
    else if (dict)
    {
        assure_file(dict);
        mo = migemo_open(dict);
        if (!word && !mode_quiet)
            printf("migemo_open(\"%s\")=%p\n", dict, mo);
        if (!mo)
        {
            fprintf(stderr, "ERROR: failed to load dictionary: %s\n", dict);
            return 1;
        }
    }
    else
    {
        const char *found = NULL;
        mo = open_first_migemo(&found, default_dicts);
        if (!word && !mode_quiet)
            printf("migemo_open(\"%s\")=%p\n", found ? found : "(N/A)", mo);
        if (!mo)
        {
            fprintf(stderr, "ERROR: failed to load default dictionaries\n");
            return 1;
        }
    }

    // Load sub-dictionaries
    if (subdict_count > 0)
    {
        for (int i = 0; i < subdict_count; ++i)
        {
            if (subdict[i] == NULL || subdict[i][0] == '\0')
                continue;
            assure_file(subdict[i]);
            int result = migemo_load(mo, MIGEMO_DICTID_MIGEMO, subdict[i]);
            if (!word && !mode_quiet)
                printf("migemo_load(%p, %d, \"%s\")=%d\n", mo,
                        MIGEMO_DICTID_MIGEMO, subdict[i], result);
        }
    }

    // Convert to static dictionary
    if (convert)
    {
        if (migemo_switch_sdict(mo, MIGEMO_SDICT_RELEASE_MDICT) == 0)
        {
            fprintf(stderr,
                    "ERROR: failed to convert the dictionary to sdict\n");
            return 1;
        }
        if (!word && !mode_quiet)
            printf("converted to sdict\n");
        if (save_sdict)
        {
            if (migemo_save_sdict(mo, save_sdict) == 0)
            {
                fprintf(stderr, "ERROR: failed to save sdict\n");
                return 1;
            }
            if (!word && !mode_quiet)
                printf("sdict saved to: %s\n", save_sdict);
        }
    }

    // Configure operators for each editor.
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

    if (word)
    {
        unsigned char *ans;

        ans = migemo_query(mo, word);
        if (ans)
            printf(mode_vim ? "%s" : "%s\n", ans);
        migemo_release(mo, ans);
    }
    else
    {
        if (!mode_quiet)
            printf("clock()=%f\n", (float)clock() / CLOCKS_PER_SEC);
        query_loop(mo, mode_quiet);
    }

    migemo_close(mo);

    return 0;
}
