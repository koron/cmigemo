/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * migemo.c -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 08-Aug-2001.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordbuf.h"
#include "wordlist.h"
#include "mnode.h"
#include "rxgen.h"
#include "romaji.h"
#include "filename.h"
#include "migemo.h"

/* migemoオブジェクト */
struct _migemo
{
    mnode* node;
    romaji* roma2hira;
    romaji* hira2kata;
    rxgen* rx;
};

/*
 * migemo interfaces
 */

    migemo*
migemo_open(char* dict)
{
    migemo *obj = NULL;
    FILE *fp = NULL;

    if (dict
	    && (fp = fopen(dict, "rt")) /* 辞書ファイルの存在確認 */
	    && (obj = (migemo*)malloc(sizeof(migemo))))
    {
#ifndef _MAX_PATH
# define _MAX_PATH 1024 /* いい加減な数値 */
#endif
	char dir[_MAX_PATH], roma_dict[_MAX_PATH], kata_dict[_MAX_PATH];

	obj->node = mnode_open(fp);
	obj->rx = rxgen_open();
	obj->roma2hira = romaji_open();
	obj->hira2kata = romaji_open();
	if (!obj->node || !obj->rx || !obj->roma2hira || !obj->hira2kata)
	{
	    migemo_close(obj);
	    obj = NULL;
	}

	filename_directory(dir, dict);
	/*printf("dir=\"%s\"\n", dir);*/
	strcpy(roma_dict, strlen(dir) ? dir : ".");
	strcpy(kata_dict, strlen(dir) ? dir : ".");
	strcat(roma_dict, "/romaji.dat");
	strcat(kata_dict, "/hira2kata.dat");
	romaji_load(obj->roma2hira, roma_dict);
	romaji_load(obj->hira2kata, kata_dict);
    }

    if (fp)
	fclose(fp); /*  辞書ファイルクローズ */
    return obj;
}

    void
migemo_close(migemo* obj)
{
    if (obj)
    {
	if (obj->hira2kata)
	    romaji_close(obj->hira2kata);
	if (obj->roma2hira)
	    romaji_close(obj->roma2hira);
	if (obj->rx)
	    rxgen_close(obj->rx);
	if (obj->node)
	    mnode_close(obj->node);
	free(obj);
    }
}

#if 0
    void
migemo_query_proc(mnode* p, void* data)
{
    wordbuf *buf = (wordbuf*)data;
    wordlist *list = p->list;

    for (; list; list = list->next)
    {
	if (wordbuf_last(buf) > 0)
	    wordbuf_add(buf, ' ');
	wordbuf_cat(buf, list->ptr);
    }
}

    unsigned char*
migemo_query(migemo* p, unsigned char* query)
{
    unsigned char* answer = NULL;
    mnode *pnode;

    /*printf("migemo_query(p=%p, query=\"%s\")\n", p, query);*/
    if (pnode = mnode_query(p->node, query))
    {
	wordbuf *pwordbuf;

	pwordbuf = wordbuf_open();
	/* バッファを用意して再帰でバッファに書き込ませる */
	mnode_traverse(pnode, migemo_query_proc, pwordbuf);
	answer = strdup(wordbuf_get(pwordbuf));
	wordbuf_close(pwordbuf);
    }
    /*printf("  pnode=%p, answer=%p\n", query, pnode, answer);*/

    return answer;
}

    void
migemo_release(migemo* p, unsigned char* string)
{
    free(string);
}
#else
/*
 * query version 2
 */

    static void
migemo_query_proc(mnode* p, void* data)
{
    rxgen *rx = (rxgen*)data;
    wordlist *list = p->list;

    for (; list; list = list->next)
	rxgen_add(rx, list->ptr);
}

/*
 * ローマ字変換が不完全だった時に、[aiueon]を補って変換してみる。
 */
    static void
migemo_query_stub(migemo* object, rxgen* rx, unsigned char* query)
{
    int len;
    char *buf;
    unsigned char candidate[] = "aiueon", *ptr;

    if (!(len = strlen(query)))
	return;
    if (!(buf = malloc(len + 2))) /* NULと拡張文字用 */
	return;
    strcpy(buf, query);
    buf[len + 1] = '\0';

    for (ptr = candidate; *ptr; ++ptr)
    {
	unsigned char *stop, *hira, *kata;

	buf[len] = *ptr;
	hira = romaji_convert(object->roma2hira, buf, &stop);

	if (!stop)
	{
	    rxgen_add(rx, hira);
	    /* 片仮名文字列を生成し候補に加える */
	    kata = romaji_convert(object->hira2kata, hira, NULL);
	    rxgen_add(rx, kata);
	    romaji_release(object->hira2kata, kata);
	}
	romaji_release(object->roma2hira, hira);
    }
}

    unsigned char*
migemo_query(migemo* object, unsigned char* query)
{
    unsigned char* answer = NULL;
    mnode *pnode;

    if (object->rx)
    {
	unsigned char *stop, *hira, *kata;

	rxgen_add(object->rx, query);

	/* 平仮名文字列を生成し候補に加える */
	hira = romaji_convert(object->roma2hira, query, &stop);
	if (!stop)
	{
	    rxgen_add(object->rx, hira);
	    /* 片仮名文字列を生成し候補に加える */
	    kata = romaji_convert(object->hira2kata, hira, NULL);
	    rxgen_add(object->rx, kata);
	    romaji_release(object->hira2kata, kata);
	}
	else
	    migemo_query_stub(object, object->rx, query);
	romaji_release(object->roma2hira, hira);

	/* バッファを用意して再帰でバッファに書き込ませる */
	if (pnode = mnode_query(object->node, query))
	    mnode_traverse(pnode, migemo_query_proc, object->rx);

	answer = rxgen_generate(object->rx);
	rxgen_reset(object->rx);
    }

    return answer;
}

    void
migemo_release(migemo* p, unsigned char* string)
{
    rxgen_release(NULL, string);
}
#endif

    int
migemo_set_operator(migemo* object, int index, unsigned char* op)
{
    return object ? rxgen_set_operator(object->rx, index, op) : 1;
}

    const unsigned char*
migemo_get_operator(migemo* object, int index)
{
    return object ? rxgen_get_operator(object->rx, index) : NULL;
}

    void
migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc)
{
    if (object)
	rxgen_setproc_char2int(object->rx, (RXGEN_PROC_CHAR2INT)proc);
}

    void
migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc)
{
    if (object)
	rxgen_setproc_int2char(object->rx, (RXGEN_PROC_INT2CHAR)proc);
}
