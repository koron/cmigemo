// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// migemo.h -
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

#if defined(_WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN32__)
# define MIGEMO_CALLTYPE __stdcall
#else
# define MIGEMO_CALLTYPE
#endif

#define MIGEMO_VERSION "1.5"

// for migemo_load()
#define MIGEMO_DICTID_INVALID   0
#define MIGEMO_DICTID_MIGEMO    1
#define MIGEMO_DICTID_ROMA2HIRA 2
#define MIGEMO_DICTID_HIRA2KATA 3
#define MIGEMO_DICTID_HAN2ZEN   4
#define MIGEMO_DICTID_ZEN2HAN   5

// for migemo_set_operator()/migemo_get_operator().  see: rxgen.h
#define MIGEMO_OPINDEX_OR         0
#define MIGEMO_OPINDEX_NEST_IN    1
#define MIGEMO_OPINDEX_NEST_OUT   2
#define MIGEMO_OPINDEX_SELECT_IN  3
#define MIGEMO_OPINDEX_SELECT_OUT 4
#define MIGEMO_OPINDEX_NEWLINE    5

// see: rxgen.h
typedef int (*MIGEMO_PROC_CHAR2INT)(const unsigned char *, unsigned int *);
typedef int (*MIGEMO_PROC_INT2CHAR)(unsigned int, unsigned char *);

/// Migemo object. Created by migemo_open() and destroyed by migemo_close.
typedef struct migemo migemo;

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// Life cycle management

/// Create a Migemo object.
///
/// Automatically loads standard conversion tables if present in the same
/// directory as the specified dictionary.
/// @param dict Path to dictionary file. If NULL, no dictionary is loaded.
/// @return Pointer to created object, or NULL on failure.
migemo *MIGEMO_CALLTYPE migemo_open(const char *dict);

/// Destroy a Migemo object and free its resources.
/// @param object Object to destroy.
void MIGEMO_CALLTYPE migemo_close(migemo *object);

//////////////////////////////////////////////////////////////////////////////
// Query

/// Generate a regular expression pattern from a Romaji query.
///
/// NOTE: The returned string is dynamically allocated. You MUST release it
/// using migemo_release() after use to avoid memory leaks.
/// @param object Migemo object.
/// @param query Query string in Romaji.
/// @return Generated regex pattern string.
unsigned char *MIGEMO_CALLTYPE migemo_query(
        migemo *object, const unsigned char *query);

/// Free memory allocated by migemo_query().
/// @param object Migemo object.
/// @param string Pointer to string to free.
void MIGEMO_CALLTYPE migemo_release(migemo *object, unsigned char *string);

//////////////////////////////////////////////////////////////////////////////
// Configurations

int MIGEMO_CALLTYPE migemo_set_operator(
        migemo *object, int index, const unsigned char *op);
const unsigned char *MIGEMO_CALLTYPE migemo_get_operator(
        migemo *object, int index);
void MIGEMO_CALLTYPE migemo_setproc_char2int(
        migemo *object, MIGEMO_PROC_CHAR2INT proc);
void MIGEMO_CALLTYPE migemo_setproc_int2char(
        migemo *object, MIGEMO_PROC_INT2CHAR proc);

int MIGEMO_CALLTYPE migemo_load(
        migemo *obj, int dict_id, const char *dict_file);
int MIGEMO_CALLTYPE migemo_is_enable(migemo *obj);

#ifdef __cplusplus
}
#endif
