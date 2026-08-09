// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// wordlist.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

typedef struct wordlist wordlist;
struct wordlist
{
    unsigned char *ptr;
    wordlist *next;
};

extern size_t n_wordlist_open;
extern size_t n_wordlist_close;
extern size_t n_wordlist_total;

#ifdef __cplusplus
extern "C" {
#endif

wordlist *wordlist_new(const unsigned char *ptr, size_t len);
void wordlist_destroy(wordlist *p);

#ifdef __cplusplus
}
#endif
