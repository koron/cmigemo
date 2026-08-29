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
    -   CMake (3.21 or above)
    -   C11 compiler and binutils equivalents (gcc, clang, msvc or so)
-   Building the dictionary
    -   CMake (3.21 or above)
    -   curl
    -   iconv
    -   gzip
    -   perl

    The dictionary is built from
    [`SKK-JISYO.L`](https://skk-dev.github.io/dict/)

-   Profiling
    -   gprof

## Build

```
$ cmake -B build
$ cmake --build build
```

### Build Options & Packaging

The following options and components are available for selective building and
installation (e.g., for distribution packaging):

-   `BUILD_DICT` (CMake Option, default: `ON`):

    Set to `OFF` (`cmake -B build -DBUILD_DICT=OFF`) to skip dictionary
    generation and its external build dependencies (`perl`, `iconv`, `gzip`,
    etc.).

-   Target `dictionaries`:

    Builds all dictionary encodings at once:

    ```bash
    cmake --build build --target dictionaries
    ```

-   Install Components:

    Allows installing core files or dictionary files separately:

    ```bash
    # Install core binary, library, and headers only
    cmake --install build --component core

    # Install generated dictionary files only
    cmake --install build --component dict
    ```

## Static Dictionary (sdict)

C/Migemo supports a pre-compiled binary dictionary format called **sdict**
(Static Dictionary).

### Benefits

-   Fast Startup: Drastically reduces initialization/load time
    (e.g., from ~50ms down to ~4ms).
-   Memory Efficiency: Significantly reduces memory footprint
    (e.g., from ~18MB down to ~8MB).
-   Improved Performance: Enhances query execution speed due to better CPU
    cache locality.

### CLI Options

`cmigemo` provides the following options for `sdict`:

-   `--convert`: Converts loaded dynamic dictionaries to `sdict` in-memory and
    continues execution. Useful for testing `sdict` performance quickly.
-   `--save <dict>`: Saves the converted `sdict` to a specified file path
    (implies `--convert`).
-   `--sdict <dict>`: Loads a pre-compiled `sdict` file directly
    (conflicts with `-d`/`-s`/`-u` and `--convert`/`--save`).

### Trying sdict in Development

```bash
$ make

# Generate and save an sdict file (using -w '' to exit immediately after saving)
$ ./build/bin/cmigemo --save build/dict/utf-8/migemo-sdict -w ''

# Launch cmigemo using the saved sdict
$ ./build/bin/cmigemo --sdict build/dict/utf-8/migemo-sdict
```

## Vim plugin

If you want to use C/Migemo only as a vim plugin,
use https://github.com/haya14busa/vim-migemo

## Licenses

C/Migemo is distributed under the MIT License.

The built dictionary is subject to the GPL, in accordance with the license of
the source SKK-JISYO.L.
