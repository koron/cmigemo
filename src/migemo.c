/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * migemo.c -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 11-Aug-2001.
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

/*
 * 既存のmigemoオブジェクトに辞書ファイルを追加読込する。
 */
    int
migemo_load(migemo* obj, char* dict, char* roma_dict, char* kata_dict)
{
    int result = 0;
    FILE *fp;

    if (!obj)
	return result;

    /* migemo辞書読み込み */
    if (dict && (fp = fopen(dict, "rt")))
    {
	obj->node = mnode_load(obj->node, fp);
	fclose(fp);
	result |= 1;
    }

    /* ローマ字辞書読み込み */
    if (roma_dict)
    {
	romaji_load(obj->roma2hira, roma_dict);
	result |= 2;
    }

    /* カタカナ辞書読み込み */
    if (kata_dict)
    {
	romaji_load(obj->hira2kata, kata_dict);
	result |= 4;
    }

    return result;
}

/*
 * (dict == NULL)として辞書を読み込ませないことも可能
 */
    migemo*
migemo_open(char* dict)
{
    migemo *obj;

    /* migemoオブジェクトと各メンバを構築 */
    if (!(obj = (migemo*)malloc(sizeof(migemo))))
	return obj;
    obj->node = mnode_open(NULL);
    obj->rx = rxgen_open();
    obj->roma2hira = romaji_open();
    obj->hira2kata = romaji_open();
    if (!obj->rx || !obj->roma2hira || !obj->hira2kata)
    {
	migemo_close(obj);
	return obj = NULL;
    }

    /* デフォルトmigemo辞書が指定されていたらローマ字とカタカナ辞書も探す */
    if (dict)
    {
#ifndef _MAX_PATH
# define _MAX_PATH 1024 /* いい加減な数値 */
#endif
	char dir[_MAX_PATH], roma_dict[_MAX_PATH], kata_dict[_MAX_PATH];

	filename_directory(dir, dict);
	strcpy(roma_dict, strlen(dir) ? dir : ".");
	strcpy(kata_dict, strlen(dir) ? dir : ".");
	strcat(roma_dict, "/romaji.dat");
	strcat(kata_dict, "/hira2kata.dat");

	migemo_load(obj, dict, roma_dict, kata_dict);
    }
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
 * バッファを用意してmnodeに再帰で書き込ませる
 */
    static void
add_mnode_query(migemo* object, unsigned char* query)
{
    mnode *pnode;

    if (pnode = mnode_query(object->node, query))
	mnode_traverse(pnode, migemo_query_proc, object->rx);
}

    static int
add_roma(migemo* object, unsigned char* query)
{
    unsigned char *stop, *hira, *kata;

    hira = romaji_convert(object->roma2hira, query, &stop);
    if (!stop)
    {
	rxgen_add(object->rx, hira);
	/* 平仮名による辞書引き */
	add_mnode_query(object, hira);
	/* 片仮名文字列を生成し候補に加える */
	kata = romaji_convert(object->hira2kata, hira, NULL);
	rxgen_add(object->rx, kata);
	/* カタカナによる辞書引き */
	add_mnode_query(object, kata);
	romaji_release(object->hira2kata, kata); /* カタカナ解放 */
    }
    romaji_release(object->roma2hira, hira); /* 平仮名解放 */

    return stop ? 1 : 0;
}

/*
 * ローマ字変換が不完全だった時に、[aiueo]および"xn"と"xtu"を補って変換して
 * みる。
 */
    static void
add_dubious_roma(migemo* object, rxgen* rx, unsigned char* query)
{
    int len;
    char *buf;
    unsigned char candidate[] = "aiueo", *ptr;

    if (!(len = strlen(query)))
	return;
    if (!(buf = malloc(len + 1 + 3))) /* NULと拡張文字用(最長:xtu) */
	return;
    strcpy(buf, query);
    buf[len + 1] = '\0';

    if (!strchr(candidate, buf[len - 1]))
    {
	/* [aiueo]を順番に補う */
	for (ptr = candidate; *ptr; ++ptr)
	{
	    buf[len] = *ptr;
	    add_roma(object, buf);
	}
	/* 未確定単語の長さが2未満か、未確定文字の直前が拇印ならば… */
	if (len < 2 || strchr(candidate, buf[len - 2]))
	{
	    if (buf[len - 1] == 'n')
	    {
		/* 「ん」を補ってみる */
		strcpy(&buf[len - 1], "xn");
		add_roma(object, buf);
	    }
	    else
	    {
		/* 「っ」を補ってみる */
		strcpy(&buf[len - 1], "xtu");
		add_roma(object, buf);
	    }
	}
    }

    free(buf);
}

    unsigned char*
migemo_query(migemo* object, unsigned char* query)
{
    unsigned char* answer = NULL;

    if (object && object->rx && query)
    {
	/* 平仮名、カタカナ、及びそれによる辞書引き追加 */
	if (add_roma(object, query))
	    add_dubious_roma(object, object->rx, query);

	/* query自信はもちろん候補に加える */
	rxgen_add(object->rx, query);
	/* queryそのものによる辞書引き */
	add_mnode_query(object, query);

	/* 検索パターン(正規表現)生成 */
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

    int
migemo_is_enable(migemo* obj)
{
    return (obj && obj->node) ? 1 : 0;
}

#if 1
/*
 * 主にデバッグ用の隠し関数
 */

    void
migemo_print(migemo* object)
{
    if (object)
	mnode_print(object->node, NULL);
}
#endif
