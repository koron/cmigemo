# C/Migemo library

C/Migemo is regexp pattern generator/expander.

C/Migemo is a C-language implementation of Migemo (Ruby/Migemo). Software
utilizing the C/Migemo library gains the ability to perform incremental
Japanese searches using Romaji input. Because it is implemented in C, it is
easier to integrate with the vast amount of software written in C compared to
the original Ruby/Migemo, and —presumably— it offers improved execution speed.

The original Migemo is a tool devised by Satoru Takabayashi that converts
Romaji input into regular expressions for Japanese text searching. It was
implemented using Ruby and Emacs Lisp to enable incremental Japanese searches
in Emacs while typing in Romaji.

## Getting Started

### Build

```console
$ cmake -B build
$ cmake --build build
```

### Test

```console
$ cmake --build build --target test
```

### Profiling

```console
$ cmake --build build --target profile
```

See `build/profile.log` for the profiling result.

### Clean

```console
$ cmake --build build --target clean
```

Or just `rm -rf build`.

## Pre-requirements

-   Building the program
    -   CMake (3.16 or above)
    -   C11 compiler and binutils equivalents (gcc, clang, msvc or so)
-   Building the dictionary
    -   CMake (3.16 or above)
    -   iconv
    -   gzip
    -   perl

    The dictionary is built from
    [`SKK-JISYO.L`](https://skk-dev.github.io/dict/)

-   Profiling
    -   gperf

## Vim plugin

If you want to use C/Migemo only as a vim plugin,
use https://github.com/haya14busa/vim-migemo

## Licenses

C/Migemo is distributed under the MIT License.

The built dictionary is subject to the GPL, in accordance with the license of
the source SKK-JISYO.L.
