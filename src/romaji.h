/* vi:set ts=8 sts=4 sw=4 tw=0: */
/*
 * romaji.h - ƒ[ƒ}š•ÏŠ·
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 15-May-2002.
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
void romaji_release(romaji* object, unsigned char* string);

#ifdef __cplusplus
}
#endif

#endif /* ROMAJI_H */
