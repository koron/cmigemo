/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * migemo.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 04-Aug-2001.
 */

#ifndef MIGEMO_H
#define MIGEMO_H

#include "cdecl.h"

typedef struct _migemo migemo;

C_DECL_BEGIN();
migemo* migemo_open(char* dict);
void migemo_close(migemo* p);
unsigned char* migemo_query(migemo* p, unsigned char* query);
void migemo_release(migemo* p, unsigned char* string);
C_DECL_END();

#endif /* MIGEMO_H */
