/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordlist.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 21-Jan-2002.
 */

#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

int n_wordlist_new	= 0;
int n_wordlist_delete	= 0;
int n_wordlist_total	= 0;

    wordlist*
wordlist_new_len(unsigned char* ptr, int len)
{
#if 1
    wordlist *p;

    ++n_wordlist_new;
    if (ptr && (p = (wordlist*)malloc(sizeof(wordlist))))
    {
#if 1
	/*
	 * ほぼstrdup()と等価な実装。単語の保存に必要な総メモリを知りたいの
	 * で。
	 */
	int len = strlen(ptr) + 1;

	n_wordlist_total += len;
	if (p->ptr = malloc(len))
	    memcpy(p->ptr, ptr, len);
	p->next = NULL;
#else
	p->ptr = strdup(ptr);
	p->next = NULL;
#endif
    }
    return p;
#else
    if (ptr && len > 1)
    {
	wordlist *p;

	if (p = (wordlist*)malloc(sizeof(*p) + len))
	{
	    ++n_wordlist_new;
	    n_wordlist_total += len;
	    p->ptr  = (char*)(p + 1);
	    p->next = NULL;
	    memcpy(p->ptr, ptr, len);
	}
	return p;
    }
    return NULL;
#endif
}

    wordlist*
wordlist_new(unsigned char* ptr)
{
    return ptr ? wordlist_new_len(ptr, strlen(ptr) + 1) : NULL;
}

    void
wordlist_delete(wordlist* p)
{
    while (p)
    {
	wordlist *next = p->next;

	++n_wordlist_delete;
	//free(p->ptr);
	free(p);
	p = next;
    }
}
