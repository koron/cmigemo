// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordlist.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include "common.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "wordlist.h"

wordlist *
wordlist_new(const unsigned char *p, size_t len)
{
    if (!p)
        return NULL;

    wordlist *wl = (wordlist *)malloc(sizeof(wordlist) + len + 1);
    if (!wl)
        return NULL;

    wl->next = NULL;

    if (len > 0)
        memcpy(wl->ptr, p, len);
    wl->ptr[len] = '\0';

    return wl;
}

void
wordlist_destroy(wordlist *wl)
{
    while (wl)
    {
        wordlist *next = wl->next;
        free(wl);
        wl = next;
    }
}

bool
wordlist_contains(wordlist *wl, const unsigned char *str)
{
    for (; wl; wl = wl->next)
        if (!strcmp(wl->ptr, str))
            return true;
    return false;
}
