/* vi:set ts=8 sts=4 sw=4 tw=0: */
/*
 * romaji.c - ローマ字変換
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 21-Jan-2002.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wordbuf.h"
#include "romaji.h"

#if defined(_MSC_VER) || defined(__GNUC__)
# define INLINE __inline
#else
# define INLINE 
#endif

#define ROMANODE_VALUE_MAXLEN 7
#define ROMAJI_KEY_MAXLEN 8
#define ROMAJI_FIXKEY_N 'n'
#define ROMAJI_FIXKEY_XN "xn"
#define ROMAJI_FIXKEY_XTU "xtu"
#define ROMAJI_FIXKEY_NONXTU "aiueon"

/*
 * romanode interfaces
 */

typedef struct _romanode romanode;
struct _romanode
{
    unsigned char key;
    unsigned char value[ROMANODE_VALUE_MAXLEN];
    romanode* next;
    romanode* child;
};

int n_romanode_new = 0;
int n_romanode_delete = 0;

    INLINE static romanode*
romanode_new()
{
    ++n_romanode_new;
    return (romanode*)calloc(1, sizeof(romanode));
}

    static void
romanode_delete(romanode* node)
{
    while (node)
    {
	romanode* child = node->child;
	if (node->next)
	    romanode_delete(node->next);
	free(node);
	node = child;
	++n_romanode_delete;
    }
}

    static romanode**
romanode_dig(romanode** ref_node, unsigned char* key)
{
    if (!ref_node || !key || key[0] == '\0')
	return NULL;

    while (1)
    {
	if (!*ref_node)
	{
	    if (!(*ref_node = romanode_new()))
		return NULL;
	    (*ref_node)->key = *key;
	}

	if ((*ref_node)->key == *key)
	{
	    (*ref_node)->value[0] = '\0';
	    if (!*++key)
		break;
	    ref_node = &(*ref_node)->child;
	}
	else
	    ref_node = &(*ref_node)->next;
    }

    if ((*ref_node)->child)
    {
	romanode_delete((*ref_node)->child);
	(*ref_node)->child = 0;
    }
    return ref_node;
}

    static void
romanode_print_stub(romanode* node, unsigned char* p)
{
    static unsigned char buf[256];

    if (!p)
	p = &buf[0];
    p[0] = node->key;
    p[1] = '\0';
    if (node->value[0])
	printf("%s=%s\n", buf, node->value);
    if (node->child)
	romanode_print_stub(node->child, p + 1);
    if (node->next)
	romanode_print_stub(node->next, p);
}

    static void
romanode_print(romanode* node)
{
    if (!node)
	return;
    romanode_print_stub(node, NULL);
}

    static romanode*
romanode_query(romanode* node, unsigned char* key, int* skip)
{
    int nskip = 0;

    if (node && key && *key)
    {
	while (1)
	{
	    if (*key != node->key)
		node = node->next;
	    else
	    {
		++nskip;
		if (node->value[0])
		    break;
		if (!*++key)
		{
		    nskip = 0;
		    break;
		}
		node = node->child;
	    }

	    if (!node)
	    {
		nskip = 1;
		break;
	    }
	}
    }

    if (skip)
	*skip = nskip;
    return node;
}

/*
 * romaji interfaces
 */

struct _romaji
{
    romanode* node;
    unsigned char fixvalue_xn[ROMANODE_VALUE_MAXLEN];
    unsigned char fixvalue_xtu[ROMANODE_VALUE_MAXLEN];
};

    static unsigned char*
strdup_lower(unsigned char* string)
{
    unsigned char *out = strdup(string), *tmp;

    if (out)
	for (tmp = out; *tmp; ++tmp)
	    *tmp = tolower(*tmp);
    return out;
}

    romaji*
romaji_open()
{
    return (romaji*)calloc(1, sizeof(romaji));
}

    void
romaji_close(romaji* object)
{
    if (object)
    {
	if (object->node)
	    romanode_delete(object->node);
	free(object);
    }
}

    int
romaji_add_table(romaji* object, unsigned char* key, unsigned char* value)
{
    int len;
    unsigned char* key_lower;
    romanode **ref_node;

    if (!object || !key || !value)
	return 1; /* Unexpected error */

    len = strlen(value);
    if (len == 0 || len >= ROMANODE_VALUE_MAXLEN)
	return 2; /* Too short/long value string */

    len = strlen(key);
    if (len == 0 || len >= ROMAJI_KEY_MAXLEN)
	return 3; /* Too short/long key string */

    /* keyは小文字に変換する */
    if (!(key_lower = strdup_lower(key)))
	return 4;

    if (!(ref_node = romanode_dig(&object->node, key_lower)))
    {
	free(key_lower);
	return 5; /* Memory exhausted */
    }
    /*printf("romaji_add_table(\"%s\", \"%s\")\n", key_lower, value);*/
    strcpy((*ref_node)->value, value);

    /* 「ん」と「っ」は保存しておく */
    if (!strcmp(key_lower, ROMAJI_FIXKEY_XN))
    {
	/*fprintf(stderr, "XN: key_lower=%s, value=%s\n", key_lower, value);*/
	strcpy(object->fixvalue_xn, value);
    }
    if (!strcmp(key_lower, ROMAJI_FIXKEY_XTU))
    {
	/*fprintf(stderr, "XTU: key_lower=%s, value=%s\n", key_lower, value);*/
	strcpy(object->fixvalue_xtu, value);
    }

    free(key_lower);
    return 0;
}

    int
romaji_load_stub(romaji* object, FILE* fp)
{
    int mode, ch;
    wordbuf_p buf_key;
    wordbuf_p buf_value;
    
    buf_key = wordbuf_open();
    buf_value = wordbuf_open();
    if (!buf_key || !buf_value)
    {
	wordbuf_close(buf_key);
	wordbuf_close(buf_value);
	return -1;
    }

    mode = 0;
    do
    {
	ch = fgetc(fp);
	switch (mode)
	{
	    case 0:
		/* key待ちモード */
		if (ch == '#')
		    mode = 1; /* 行末まで読み飛ばしモード へ移行 */
		else if (ch != EOF && !isspace(ch))
		{
		    wordbuf_reset(buf_key);
		    wordbuf_add(buf_key, (unsigned char)ch);
		    mode = 2; /* key読み込みモード へ移行 */
		}
		break;

	    case 1:
		/* 行末まで読み飛ばしモード */
		if (ch == '\n')
		    mode = 0; /* key待ちモード へ移行 */
		break;

	    case 2:
		/* key読み込みモード */
		if (!isspace(ch))
		    wordbuf_add(buf_key, (unsigned char)ch);
		else
		    mode = 3; /* value待ちモード へ移行 */
		break;

	    case 3:
		/* value待ちモード */
		if (ch != EOF && !isspace(ch))
		{
		    wordbuf_reset(buf_value);
		    wordbuf_add(buf_value, (unsigned char)ch);
		    mode = 4; /* value読み込みモード へ移行 */
		}
		break;

	    case 4:
		/* value読み込みモード */
		if (ch != EOF && !isspace(ch))
		    wordbuf_add(buf_value, (unsigned char)ch);
		else
		{
		    unsigned char *key = WORDBUF_GET(buf_key);
		    unsigned char *value = WORDBUF_GET(buf_value);
		    romaji_add_table(object, key, value);
		    mode = 0;
		}
		break;
	}
    }
    while (ch != EOF);

    wordbuf_close(buf_key);
    wordbuf_close(buf_value);
    return 0;
}

    int
romaji_load(romaji* object, unsigned char* filename)
{
    int result = 0;
    FILE *fp;
   
    if (object && filename && (fp = fopen(filename, "rt")))
    {
	result = romaji_load_stub(object, fp);
	fclose(fp);
    }
    return result;
}

    unsigned char*
romaji_convert(romaji* object, unsigned char* string, unsigned char** ppstop)
{
    wordbuf_p buf = NULL;
    unsigned char *lower = NULL;
    unsigned char *answer = NULL;
    int stop = -1;

    if (object && string && (lower = strdup_lower(string))
	    && (buf = wordbuf_open()))
    {
	int i;

	for (i = 0; string[i]; )
	{
	    romanode *node;
	    int skip;

	    /* 「っ」の判定 */
	    if (object->fixvalue_xtu[0] && lower[i] == lower[i + 1]
		    && !strchr(ROMAJI_FIXKEY_NONXTU, lower[i]))
	    {
		++i;
		wordbuf_cat(buf, object->fixvalue_xtu);
		continue;
	    }

	    node = romanode_query(object->node, &lower[i], &skip);
	    if (!skip)
	    {
#if 0
		/*
		 * queryの最後がnの場合強制的に「ん」に変換したいところだ
		 * が、その場合migemoの方で[なにぬねのん]を検索できない不具
		 * 合となる。今回は泣く。
		 */
		/* 「n(子音)」を「ん(子音)」に変換 */
		if (lower[i] == ROMAJI_FIXKEY_N && object->fixvalue_xn[0])
		{
		    ++i;
		    wordbuf_cat(buf, object->fixvalue_xn);
		}
#endif
		if (string[i])
		{
		    stop = WORDBUF_LEN(buf);
		    wordbuf_cat(buf, &string[i]);
		}
		break;
	    }
	    else if (!node)
	    {
		/* 「n(子音)」を「ん(子音)」に変換 */
		if (skip == 1 && lower[i] == ROMAJI_FIXKEY_N
			&& object->fixvalue_xn[0])
		{
		    ++i;
		    wordbuf_cat(buf, object->fixvalue_xn);
		}
		else
		    while (skip--)
			wordbuf_add(buf, string[i++]);
	    }
	    else
	    {
		i += skip;
		wordbuf_cat(buf, node->value);
	    }
	}
	answer = strdup(WORDBUF_GET(buf));
    }
    if (ppstop)
	*ppstop = (stop >= 0) ? answer + stop : NULL;

    if (lower)
	free(lower);
    if (buf)
	wordbuf_close(buf);
    return answer;
}

    void
romaji_release(romaji* object, unsigned char* string)
{
    free(string);
}

#if 0
/*
 * main
 */

    void
query_loop(romaji* object, romaji* hira2kata)
{
    char buf[256];

    while (1)
    {
	printf("QUERY: ");
	if (!gets(buf))
	    break;
#if 0
	{
	    int skip;
	    romanode* node;

	    node = romanode_query(object->node, buf, &skip);
	    printf("node=%p, skip=%d\n", node, skip);
	    if (node)
		printf("  value=%s\n", node->value);
	}
#else
	{
	    unsigned char *stop;
	    unsigned char *hira;
	    if (hira = romaji_convert(object, buf, &stop))
	    {
		unsigned char* kata;

		printf("  hira=%s, stop=%s\n", hira, stop);
		if (kata = romaji_convert(hira2kata, hira, &stop))
		{
		    printf("  kata=%s, stop=%s\n", kata, stop);
		    romaji_release(hira2kata, kata);
		}
		romaji_release(object, hira);
	    }

	}
#endif
    }
}

    int
main(int argc, char** argv)
{
    romaji *object, *hira2kata;

    object = romaji_open();
    hira2kata = romaji_open();

    if (object && hira2kata)
    {
#if 0
	romaji_add_table(object, "a", "あ");
	romaji_add_table(object, "i", "い");
	romaji_add_table(object, "u", "う");
	romaji_add_table(object, "e", "え");
	romaji_add_table(object, "o", "お");
#else
	romaji_load(object, "romaji.dat");
	romaji_load(hira2kata, "hira2kata.dat");
#if 0
	romanode_print(object->node);
	romanode_print(hira2kata->node);
#else
	query_loop(object, hira2kata);
#endif
#endif
    }

    if (hira2kata)
	romaji_close(hira2kata);
    if (object)
	romaji_close(object);

    /* 実行時情報表示 */
    fprintf(stderr, "sizeof(romanode)=%d\n", sizeof(romanode));
    fprintf(stderr, "sizeof(romaji)=%d\n", sizeof(romaji));
    fprintf(stderr, "n_romanode_new=%d\n", n_romanode_new);
    fprintf(stderr, "n_romanode_delete=%d\n", n_romanode_delete);
    fprintf(stderr, "n_wordbuf_open=%d\n", n_wordbuf_open);
    fprintf(stderr, "n_wordbuf_close=%d\n", n_wordbuf_close);

    return 0;
}
#endif
