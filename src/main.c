/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * main.c - migemoライブラリテストドライバ
 *
 * Written By:  Muraoka Taro  <koron@tka.att.en.jp>
 * Last Change: 16-May-2002.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "migemo.h"

#define MIGEMODICT_NAME "migemo-dict"

/*
 * main
 */

    int
query_loop(migemo* p, int quiet)
{
    while (!feof(stdin))
    {
	unsigned char buf[256], *ans;

	if (!quiet)
	    printf("QUERY: ");
	/* gets()を使っていたがfgets()に変更 */
	if (!fgets(buf, sizeof(buf), stdin))
	{
	    if (!quiet)
		printf("\n");
	    break;
	}
	/* 改行はNUL文字に置き換える */
	if ((ans = strchr(buf, '\n')) != NULL)
	    *ans = '\0';

	ans = migemo_query(p, buf);
	if (ans)
	    printf(quiet ? "%s\n" : "PATTERN: %s\n", ans);
	migemo_release(p, ans);
    }
    return 0;
}

    int
main(int argc, char** argv)
{
    int mode_vim = 0;
    int mode_quiet = 0;
    char* dict = NULL;
    migemo *pmigemo;
    FILE *fplog = stdout;
    char *word = NULL;

    while (*++argv)
    {
	if (0)
	    ;
	else if (!strcmp("--vim", *argv) || !strcmp("-v", *argv))
	    mode_vim = 1;
	else if (argv[1] && (!strcmp("--dict", *argv) || !strcmp("-d", *argv)))
	    dict = *++argv;
	else if (argv[1] && (!strcmp("--word", *argv) || !strcmp("-w", *argv)))
	    word = *++argv;
	else if (!strcmp("--quiet", *argv) || !strcmp("-q", *argv))
	    mode_quiet = 1;
    }

#ifdef _PROFILE
    fplog = fopen("exe.log", "wt");
#endif

    /* 辞書をカレントディレクトリと1つ上のディレクトリから捜す */
    if (!dict)
    {
	pmigemo = migemo_open("./dict/" MIGEMODICT_NAME);
	if (!word && !mode_quiet)
	{
	    fprintf(fplog, "migemo_open(%s)=%p\n",
		    "./dict/" MIGEMODICT_NAME, pmigemo);
	}
	if (!pmigemo || !migemo_is_enable(pmigemo))
	{
	    migemo_close(pmigemo); /* NULLをcloseしても問題はない */
	    pmigemo = migemo_open("../dict/" MIGEMODICT_NAME);
	    if (!word && !mode_quiet)
	    {
		fprintf(fplog, "migemo_open(%s)=%p\n",
			"../dict/" MIGEMODICT_NAME, pmigemo);
	    }
	}
    }
    else
	pmigemo = migemo_open(dict);

    if (!pmigemo)
	return 1;
    else
    {
	if (mode_vim)
	{
	    migemo_set_operator(pmigemo, MIGEMO_OPINDEX_OR, "\\|");
	    migemo_set_operator(pmigemo, MIGEMO_OPINDEX_NEST_IN, "\\%(");
	    migemo_set_operator(pmigemo, MIGEMO_OPINDEX_NEST_OUT, "\\)");
	    migemo_set_operator(pmigemo, MIGEMO_OPINDEX_NEWLINE, "\\_s*");
	}
#ifndef _PROFILE
	if (word)
	{
	    unsigned char *ans;

	    ans = migemo_query(pmigemo, word);
	    if (ans)
		fprintf(fplog, mode_vim ? "%s" : "%s\n", ans);
	    migemo_release(pmigemo, ans);
	}
	else
	{
	    if (!mode_quiet)
		printf("clock()=%f\n", (float)clock() / CLOCKS_PER_SEC);
	    query_loop(pmigemo, mode_quiet);
	}
#else
	/* プロファイル用 */
	{
	    unsigned char *ans;

	    ans = migemo_query(pmigemo, "a");
	    if (ans)
		fprintf(fplog, "  [%s]\n", ans);
	    migemo_release(pmigemo, ans);

	    ans = migemo_query(pmigemo, "k");
	    if (ans)
		fprintf(fplog, "  [%s]\n", ans);
	    migemo_release(pmigemo, ans);
	}
#endif
	migemo_close(pmigemo);
    }

    if (fplog != stdout)
	fclose(fplog);
    return 0;
}
