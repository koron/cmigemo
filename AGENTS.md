# AGENTS.md

## Requirements

- **Build**: CMake (3.16+), C11 compiler (GCC, Clang, MSVC)
- **Dictionary Generation**: `curl`, `iconv`, `gzip`, `perl`
- **Profiling**: `gperf` (GCC/Clang only)

## Build

- Configure: `cmake -B build` (default type: `Release`; for debug: `-DCMAKE_BUILD_TYPE=Debug`)
- Build: `cmake --build build --parallel`
- Test: `cmake --build build --target test` (runs CTest)
- Profiling: `cmake --build build --target profile` (requires `gperf`; GCC/Clang only, not MSVC)
- Package: `cmake --build build --target package` (CPack ZIP on Windows, TGZ elsewhere)
- Clean: `cmake --build build --target clean` or `rm -rf build`

## Makefile Shortcuts

`make build`, `make test`, `make profile`, `make format`, `make tags`, `make package`, `make clean`, `make distclean`

## Run CLI

```console
./build/src/cmigemo -d build/dict/utf-8/migemo-dict
```

Options: `-d <dict>`, `-s <subdict>` (max 8), `-w <word>` (expand and exit), `-v`/`-e` (vim/emacs regexp style), `-q`, `-n`.

Default dictionary search order (platform-dependent):

- Windows: `./dict/cp932/`, `../dict/cp932/`, `./build/dict/cp932/`, then `./dict/`, `../dict/`, `./build/dict/`
- Non-Windows: same paths with `utf-8`

## Dictionary Generation

- Downloads `SKK-JISYO.L` from `https://skk-dev.github.io/dict/`, converts EUC-JP to UTF-8 via `iconv`, runs `dict/skk2migemo.pl` then `dict/optimize-dict.pl`
- Encodings: `utf-8`, `euc-jp`, `cp932` (in `build/dict/<encoding>/`)
- Default encoding: Windows = `cp932`, non-Windows = `utf-8`

## Test

- Executable: `test1` with one CTest suite `test_migemo1`
- Working directory: `${CMAKE_SOURCE_DIR}/test`
- Test dictionary: `test/test1/migemo-dict`
- Test subdicts: `../dict/roma2hira.dat`, `../dict/hira2kata.dat`, `../dict/han2zen.dat`, `../dict/zen2han.dat`
- Run all: `cmake --build build --target check` (runs `ctest --output-on-failure`)

## Project Structure

- `include/migemo.h` — Public C API (version 1.5)
- `src/migemo.c`, `src/rxgen.c`, `src/romaji.c`, `src/charset.c`, `src/strbuf.c`, `src/wordlist.c`, `src/mnode.c`, `src/filename.c` — Core library (`migemo` shared lib)
- `src/main.c` — CLI executable (`cmigemo`)
- `src/testdir/romaji_main.c` — Romaji conversion tool
- `src/testdir/profile_speed.c` — qspeed benchmark tool
- `dict/` — Dictionary build scripts + static `.dat` files + `migemo-dict-zh` (Chinese dict)
- `test/` — Unit test (`test1.c`)

## Coding Standards

- C/C++ formatting: `clang-format -i <file>` (LLVM style, `.clang-format` in repo root)
- CMake formatting: `.cmakefmt.yaml` (2-space indent, line_width 80)
- Editor modelines: preserve `// vim:set ts=8 sts=4 sw=4 tw=0 et:` headers in source files
- MSVC: `/utf-8`, `_CRT_SECURE_NO_WARNINGS`, `MultiThreadedDebug` runtime for testdir executables

## Install

```console
cmake --install build
```

Installs binaries/headers to standard paths; dictionaries to `${CMAKE_INSTALL_DATADIR}/cmigemo/<encoding>/`.

## Constraints

- `BUILD_TESTING=OFF` disables tests at configure time
- Profiling (`profile` target) only works with GCC/Clang; skipped on MSVC
- C11 compiler required; CMake 3.16+
