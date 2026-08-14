// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// romaji.h - Romaji conversion
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

#include "charset.h"
#include "wordlist.h"

typedef struct romaji romaji;

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
romaji *romaji_open();
void romaji_close(romaji *rj);

// Load and convert
int romaji_load(romaji *rj, const unsigned char *filename,
        CHARSET_PROC_CHAR2INT char2int);
wordlist *romaji_convert_all(romaji *rj, const unsigned char *src);

// Debug & maintenance
void romanode_print_stat(romaji *rj, const char *title);

#ifdef __cplusplus
}
#endif
