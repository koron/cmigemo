/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordlist.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 11-Aug-2001.
 */

#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

int n_wordlist_new	= 0;
int n_wordlist_delete	= 0;
int n_wordlist_total	= 0;

    wordlist*
wordlist_new(unsigned char* ptr)
{
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
}

    void
wordlist_delete(wordlist* p)
{
    while (p)
    {
	wordlist *next = p->next;

	++n_wordlist_delete;
	free(p->ptr);
	free(p);
	p = next;
    }
}
