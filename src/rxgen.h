// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// rxgen.h - regular expression generator
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#pragma once

#include "charset.h"

typedef struct rxgen rxgen;

// for rxgen_set_operator
#define RXGEN_OPINDEX_OR         0
#define RXGEN_OPINDEX_NEST_IN    1
#define RXGEN_OPINDEX_NEST_OUT   2
#define RXGEN_OPINDEX_SELECT_IN  3
#define RXGEN_OPINDEX_SELECT_OUT 4
#define RXGEN_OPINDEX_NEWLINE    5

#ifdef __cplusplus
extern "C" {
#endif

// Life cycle management
rxgen *rxgen_open();
void rxgen_close(rxgen *rx);

// Regexp pattern generation
int rxgen_add(rxgen *rx, const unsigned char *word);
unsigned char *rxgen_generate(rxgen *rx);
void rxgen_release(rxgen *rx, unsigned char *s);
void rxgen_reset(rxgen *rx);

// Configuration
void rxgen_setproc_char2int(rxgen *rx, CHARSET_PROC_CHAR2INT proc);
void rxgen_setproc_int2char(rxgen *rx, CHARSET_PROC_INT2CHAR proc);
void rxgen_set_escape_chars(rxgen *rx, const unsigned char *chars);
int rxgen_set_operator(rxgen *rx, int index, const unsigned char *op);
const unsigned char *rxgen_get_operator(rxgen *rx, int index);

#ifdef __cplusplus
}
#endif
