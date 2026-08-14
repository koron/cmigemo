// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji.h - Romaji conversion
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

#include "charset.h"
#include "wordlist.h"

typedef struct romaji romaji;

typedef int (*romaji_proc_char2int)(const unsigned char *, unsigned int *);
#define ROMAJI_PROC_CHAR2INT romaji_proc_char2int

#ifdef __cplusplus
extern "C" {
#endif

romaji *romaji_open();
void romaji_close(romaji *object);
int romaji_load(romaji *object, const unsigned char *filename,
        CHARSET_PROC_CHAR2INT char2int);

void romaji_setproc_char2int(romaji *object, ROMAJI_PROC_CHAR2INT proc);
void romaji_set_verbose(romaji *object, int level);

void romanode_print_stat(romaji *obj, const char *title);

wordlist *romaji_convert_all(romaji *object, const unsigned char *src);

#ifdef __cplusplus
}
#endif
