/* vim:set ts=8 sts=4 sw=4 tw=0 */
/*
 * migemo.h -
 *
 * Written By:  Muraoka Taro <koron@tka.att.ne.jp>
 * Last Change: 11-Aug-2001.
 */

#ifndef MIGEMO_H
#define MIGEMO_H

#if !defined(C_DECL_BEGIN) && !defined(C_DECL_END)
# ifdef __cplusplus
#  define C_DECL_BEGIN() extern "C" {
#  define C_DECL_END() }
# else
#  define C_DECL_BEGIN()
#  define C_DECL_END()
# endif
#endif

/* for migemo_set_operator()/migemo_get_operator().  see: rxgen.h */
#define MIGEMO_OPINDEX_OR 0
#define MIGEMO_OPINDEX_NEST_IN 1
#define MIGEMO_OPINDEX_NEST_OUT 2
#define MIGEMO_OPINDEX_SELECT_IN 3
#define MIGEMO_OPINDEX_SELECT_OUT 4
#define MIGEMO_OPINDEX_NEWLINE 5

/* see: rxgen.h */
typedef int (*MIGEMO_PROC_CHAR2INT)(unsigned char*, unsigned int*);
typedef int (*MIGEMO_PROC_INT2CHAR)(unsigned int, unsigned char*);

typedef struct _migemo migemo;

C_DECL_BEGIN();
migemo* migemo_open(char* dict);
void migemo_close(migemo* object);
unsigned char* migemo_query(migemo* object, unsigned char* query);
void migemo_release(migemo* object, unsigned char* string);

int migemo_set_operator(migemo* object, int index, unsigned char* op);
const unsigned char* migemo_get_operator(migemo* object, int index);
void migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc);
void migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc);

int migemo_load(migemo* obj, char* dict, char* roma_dict, char* kata_dict);
int migemo_is_enable(migemo* obj);
C_DECL_END();

#endif /* MIGEMO_H */
