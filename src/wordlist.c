// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordlist.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "wordlist.h"

int n_wordlist_open = 0;
int n_wordlist_close = 0;
int n_wordlist_total = 0;

wordlist *
wordlist_new(const unsigned char *ptr, int len)
{
    if (!ptr || len < 0)
        return NULL;

    wordlist *p = (wordlist *)malloc(sizeof(*p) + len + 1);
    if (!p)
        return NULL;

    p->ptr = (char *)(p + 1);
    p->next = NULL;

    // Implementation nearly equivalent to strdup(). Reimplemented manually
    // because we need to know the total memory required to store the word.
    memcpy(p->ptr, ptr, len);
    p->ptr[len] = '\0';

    ++n_wordlist_open;
    n_wordlist_total += len;

    return p;
}

void
wordlist_destroy(wordlist *p)
{
    while (p)
    {
        wordlist *next = p->next;

        ++n_wordlist_close;
        free(p);
        p = next;
    }
}
