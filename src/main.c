/*
 * main.c - migemoライブラリテストドライバ
 *
 * Written By:  Muraoka Taro  <koron@tka.att.en.jp>
 * Last Change: 21-Jan-2002.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "dbg.h"
#include "wordbuf.h"
#include "wordlist.h"
#include "mnode.h"
#include "rxgen.h"
#include "migemo.h"

#define MIGEMODICT_NAME "migemo-dict"

/*
 * main
 */

    int
query_loop(migemo* p)
{
    while (!feof(stdin))
    {
	unsigned char buf[256], *ans;

	printf("QUERY: ");
	/* gets()を使っていたがfgets()に変更 */
	if (!fgets(buf, sizeof(buf), stdin))
	    break;
	/* 改行はNUL文字に置き換える */
	if (ans = strchr(buf, '\n'))
	    *ans = '\0';

	ans = migemo_query(p, buf);
	if (ans)
	    printf("PATTERN: %s\n", ans);
	migemo_release(p, ans);
    }
    return 0;
}

    int
main(int argc, char** argv)
{
#ifndef _SPLITED_MIGEMO
    void migemo_print(migemo* object);
    int mode_print = 0;
#endif
    int mode_vim = 0;
    char* dict = NULL;
    migemo *pmigemo;

    while (*++argv)
    {
	if (0)
	    ;
	else if (!strcmp("--vim", *argv) || !strcmp("-v", *argv))
	    mode_vim = 1;
	else if (argv[1] && (!strcmp("--dict", *argv) || !strcmp("-d", *argv)))
	    dict = *++argv;
#ifndef _SPLITED_MIGEMO
	else if (!strcmp("--print-node", *argv) || !strcmp("-p", *argv))
	    mode_print = 1;
#endif
    }

    /* 辞書をカレントディレクトリと1つ上のディレクトリから捜す */
    if (!dict)
    {
	pmigemo = migemo_open("./dict/" MIGEMODICT_NAME);
	if (!pmigemo || !migemo_is_enable(pmigemo))
	{
	    migemo_close(pmigemo); /* NULLをcloseしても問題はない */
	    pmigemo = migemo_open("../dict/" MIGEMODICT_NAME);
	}
    }
    else
	pmigemo = migemo_open(dict);

    if (!pmigemo)
	return 1;

#ifndef _SPLITED_MIGEMO
    if (mode_print)
	migemo_print(pmigemo);
#endif
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
	printf("clock()=%f\n", (float)clock() / CLOCKS_PER_SEC);
	query_loop(pmigemo);
#else
	/* プロファイル用 */
	{
	    unsigned char *ans;
	    FILE* fp = fopen("exe.log", "wt");

	    ans = migemo_query(pmigemo, "a");
	    if (ans)
		fprintf(fp, "  [%s]\n", ans);
	    migemo_release(pmigemo, ans);

	    ans = migemo_query(pmigemo, "k");
	    if (ans)
		fprintf(fp, "  [%s]\n", ans);
	    migemo_release(pmigemo, ans);

	    fclose(fp);
	}
#endif
	migemo_close(pmigemo);
    }

#ifdef FULL_LINK_TEST
    /* 統計データ表示 */
    fprintf(stderr, "n_mnode_new=      %8d\n", n_mnode_new);
    fprintf(stderr, "n_mnode_delete=   %8d\n", n_mnode_delete);
    fprintf(stderr, "n_rnode_new=      %8d\n", n_rnode_new);
    fprintf(stderr, "n_rnode_delete=   %8d\n", n_rnode_delete);
    fprintf(stderr, "n_wordbuf_open=   %8d\n", n_wordbuf_open);
    fprintf(stderr, "n_wordbuf_close=  %8d\n", n_wordbuf_close);
    fprintf(stderr, "n_wordlist_new=   %8d\n", n_wordlist_new);
    fprintf(stderr, "n_wordlist_delete=%8d\n", n_wordlist_delete);
    fprintf(stderr, "n_wordlist_total= %8d\n", n_wordlist_total);
#ifdef _MSC_VER
    _RPT1(_CRT_WARN, "n_mnode_new=      %8d\n", n_mnode_new);
    _RPT1(_CRT_WARN, "n_mnode_delete=   %8d\n", n_mnode_delete);
    _RPT1(_CRT_WARN, "n_rnode_new=      %8d\n", n_rnode_new);
    _RPT1(_CRT_WARN, "n_rnode_delete=   %8d\n", n_rnode_delete);
    _RPT1(_CRT_WARN, "n_wordbuf_open=   %8d\n", n_wordbuf_open);
    _RPT1(_CRT_WARN, "n_wordbuf_close=  %8d\n", n_wordbuf_close);
    _RPT1(_CRT_WARN, "n_wordlist_new=   %8d\n", n_wordlist_new);
    _RPT1(_CRT_WARN, "n_wordlist_delete=%8d\n", n_wordlist_delete);
    _RPT1(_CRT_WARN, "n_wordlist_total= %8d\n", n_wordlist_total);
#endif
#endif

    return 0;
}
