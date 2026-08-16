// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordlist.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

#include <stdbool.h>

typedef struct wordlist wordlist;
struct wordlist
{
    wordlist *next;
    unsigned char ptr[];
};

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
wordlist *wordlist_new(const unsigned char *ptr, size_t len);
void wordlist_destroy(wordlist *wl);

// Others
bool wordlist_contains(wordlist *wl, const unsigned char *str);

#ifdef __cplusplus
}
#endif

static inline wordlist *
wordlist_next(wordlist *wl)
{
    return wl->next;
}

static inline unsigned char *
wordlist_word(wordlist *wl)
{
    return wl->ptr;
}
