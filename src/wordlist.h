/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * wordlist.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 15-May-2002.
 */

#ifndef WORDLIST_H
#define WORDLIST_H

typedef struct _wordlist_t wordlist_t, *wordlist_p;
struct _wordlist_t
{
    unsigned char* ptr;
    wordlist_p next;
};

extern int n_wordlist_open;
extern int n_wordlist_close;
extern int n_wordlist_total;

#ifdef __cplusplus
extern "C" {
#endif

wordlist_p wordlist_open(unsigned char* ptr);
wordlist_p wordlist_open_len(unsigned char* ptr, int len);
void wordlist_close(wordlist_p p);

#ifdef __cplusplus
}
#endif

#endif /* WORDLIST_H */
