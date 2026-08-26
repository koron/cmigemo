// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// migemo_struct.h -

#pragma once

#include "charset.h"
#include "mtree.h"
#include "romaji.h"
#include "rxgen.h"
#include "stree.h"

// Migemo object
struct migemo
{
    int enable;

    mtree *mtree;
    stree *stree;

    int charset;
    romaji *roma2hira;
    romaji *hira2kata;
    romaji *han2zen;
    romaji *zen2han;
    rxgen *rx;

    CHARSET_PROC_CHAR2INT char2int;
};
