/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * wordlist.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 07-Aug-2001.
 */

#ifndef WORDLIST_H
#define WORDLIST_H

#include "cdecl.h"

typedef struct _wordlist wordlist;
struct _wordlist
{
    unsigned char* ptr;
    wordlist* next;
};

extern int n_wordlist_new;
extern int n_wordlist_delete;
extern int n_wordlist_total;

C_DECL_BEGIN();
wordlist* wordlist_new(unsigned char* ptr);
void wordlist_delete(wordlist* p);
C_DECL_END();

#endif /* WORDLIST_H */
