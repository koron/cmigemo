/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * migemo.h -
 *
 * Written By:  MURAOKA Taro <koron@tka.att.ne.jp>
 * Last Change: 28-Oct-2003.
 */

#ifndef MIGEMO_H
#define MIGEMO_H

#define MIGEMO_VERSION "1.1"

/* for migemo_load() */
#define MIGEMO_DICTID_INVALID		0
#define MIGEMO_DICTID_MIGEMO		1
#define MIGEMO_DICTID_ROMA2HIRA		2
#define MIGEMO_DICTID_HIRA2KATA		3
#define MIGEMO_DICTID_HAN2ZEN		4

/* for migemo_set_operator()/migemo_get_operator().  see: rxgen.h */
#define MIGEMO_OPINDEX_OR		0
#define MIGEMO_OPINDEX_NEST_IN		1
#define MIGEMO_OPINDEX_NEST_OUT		2
#define MIGEMO_OPINDEX_SELECT_IN	3
#define MIGEMO_OPINDEX_SELECT_OUT	4
#define MIGEMO_OPINDEX_NEWLINE		5

/* see: rxgen.h */
typedef int (*MIGEMO_PROC_CHAR2INT)(unsigned char*, unsigned int*);
typedef int (*MIGEMO_PROC_INT2CHAR)(unsigned int, unsigned char*);

/**
 * Migemoオブジェクト。migemo_open()で作成され、migemo_closeで破棄される。
 */
typedef struct _migemo migemo;

#ifdef __cplusplus
extern "C" {
#endif

migemo* migemo_open(char* dict);
void migemo_close(migemo* object);
unsigned char* migemo_query(migemo* object, unsigned char* query);
void migemo_release(migemo* object, unsigned char* string);

int migemo_set_operator(migemo* object, int index, unsigned char* op);
const unsigned char* migemo_get_operator(migemo* object, int index);
void migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc);
void migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc);

int migemo_load(migemo* obj, int dict_id, char* dict_file);
int migemo_is_enable(migemo* obj);

#ifdef __cplusplus
}
#endif

#endif /* MIGEMO_H */
