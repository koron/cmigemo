/* vi:set ts=8 sts=4 sw=4 tw=0: */
/*
 * romaji.h - ÉçÅ[É}éöïœä∑
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 29-Dec-2003.
 */

#ifndef ROMAJI_H
#define ROMAJI_H

typedef struct _romaji romaji;

#ifdef __cplusplus
extern "C" {
#endif

romaji* romaji_open();
void romaji_close(romaji* object);
int romaji_add_table(romaji* object, unsigned char* key, unsigned char* value);
int romaji_load(romaji* object, unsigned char* filename);
unsigned char* romaji_convert(romaji* object, unsigned char* string, unsigned char** ppstop);
unsigned char* romaji_convert2(romaji* object, unsigned char* string,
	unsigned char** ppstop, int ignorecase);
void romaji_release(romaji* object, unsigned char* string);

#ifdef __cplusplus
}
#endif

#endif /* ROMAJI_H */
