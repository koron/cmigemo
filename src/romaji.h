/* vi:set ts=8 sts=4 sw=4 tw=0: */
/*
 * romaji.h - ÉçÅ[É}éöïœä∑
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 07-Aug-2001.
 */

#ifndef ROMAJI_H
#define ROMAJI_H

#include "cdecl.h"

typedef struct _romaji romaji;

C_DECL_BEGIN();
romaji* romaji_open();
void romaji_close(romaji* object);
int romaji_add_table(romaji* object, unsigned char* key, unsigned char* value);
int romaji_load(romaji* object, unsigned char* filename);
unsigned char* romaji_convert(romaji* object, unsigned char* string, unsigned char** ppstop);
void romaji_release(romaji* object, unsigned char* string);
C_DECL_END();

#endif /* ROMAJI_H */
