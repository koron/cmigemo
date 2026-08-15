# AGENTS.md

## Requirements

- **Build**: CMake (3.16+), C11 compiler (GCC, Clang, MSVC)
- **Dictionary generation** (default, `BUILD_DICT=ON`): `curl`, `iconv`, `gzip`, `perl` and network access (downloads `SKK-JISYO.L`)
- **Profiling** (GCC/Clang only): `gprof`

## Build

- Configure: `cmake -B build` (default type: `Release`; for debug: `-DCMAKE_BUILD_TYPE=Debug`)
- Build: `cmake --build build --parallel`
- Artifacts: executables in `build/bin/` (`cmigemo`, `libmigemo`, `qspeed`, `romaji`, `test1`), library in `build/lib/`
- Test: `cmake --build build --target test` (CTest) or `--target check` (ctest with `--output-on-failure`)
- Profiling: `cmake --build build --target profile` (needs `gprof`; GCC/Clang only, not MSVC) -> `build/profile.log`
- Package: `cmake --build build --target package` (CPack ZIP on Windows, TGZ elsewhere)
- Clean: `cmake --build build --target clean` or `rm -rf build`

## Makefile Shortcuts

`make build`, `make test` (build + `check`), `make profile`, `make package`, `make format` (clang-format on `src test include`), `make tags`, `make clean`, `make distclean`

## Run CLI

```console
./build/bin/cmigemo -d build/dict/utf-8/migemo-dict
```

(Windows: `cmigemo.exe`; default dict encoding is `cp932` there, `utf-8` elsewhere.)

- Options: `-d <dict>`, `-s <subdict>` (max 8), `-w <word>` (expand and exit), `-v`/`-e` (vim/emacs regexp style), `-n` (no newline match), `-q`.
- Without `-d`, the binary searches (platform-dependent): `./dict/<enc>/`, `../dict/<enc>/`, `./build/dict/<enc>/`, then `./dict/`, `../dict/`, `./build/dict/`, plus the compiled-in install dir (`<DATADIR>/cmigemo/<enc>`).
- `migemo_open()` auto-loads `roma2hira.dat`, `hira2kata.dat`, `han2zen.dat`, `zen2han.dat` from the dictionary's directory; **missing sub-dictionaries are silently ignored** (src/migemo.c).

## Dictionary Generation

- Pipeline: download `SKK-JISYO.L` (EUC-JISX0213) from `https://skk-dev.github.io/dict/` -> `iconv` to UTF-8 -> `dict/skk2migemo.pl` -> `dict/optimize-dict.pl` -> per-encoding copies
- Encodings: `utf-8`, `euc-jp`, `cp932`, each built into `build/dict/<encoding>/`; default: Windows = `cp932`, non-Windows = `utf-8`
- `migemo-dict-zh` (Chinese dict) is copied only into `utf-8`; cp932 conversion tolerates iconv errors
- Skip all dictionary work (and the curl/iconv/gzip/perl requirements): `cmake -B build -DBUILD_DICT=OFF`
- Build only dictionaries: `cmake --build build --target dictionaries`

## Test

- `test/` builds executable `test1` from `main.c` + `test1.c`; one CTest suite `test_migemo1`, working directory `${PROJECT_SOURCE_DIR}/test`
- Test fixture: `test/test1/migemo-dict` (static file, not generated); the test also explicitly loads `dict/roma2hira.dat`
- New tests: add `testN.c` with `int testN(void)`, call it from `test/main.c`, add an `add_test` in `test/CMakeLists.txt`
- Run all: `cmake --build build --target check`

## Project Structure

- `include/migemo.h.in` - public C API template; generated to `build/src/include/migemo.h` at configure time; version comes from `project(VERSION)` in top-level `CMakeLists.txt` (matches CMake project version)
- `src/migemo.c`, `rxgen.c`, `romaji.c`, `charset.c`, `strbuf.c`, `wordlist.c`, `mtree.c`, `trie.c`, `filename.c` - core shared library (`migemo`)
- `src/main.c` - CLI executable (`cmigemo`)
- `src/testdir/romaji_main.c` - `romaji` tool; `profile_speed.c` - `qspeed` benchmark (drives the `profile` target)
- `dict/` - dictionary build scripts + static `.dat` sub-dictionaries + `migemo-dict-zh`
- `misc/` - reference C# wrapper and vim plugin (not built)
- `doc/` - Japanese docs + doxygen config

## Coding Standards

- C formatting: `clang-format` per `.clang-format` (LLVM-based, 4-space indent, 80 cols, return type on own line); `make format`
- CMake formatting: `cmakefmt` per `.cmakefmt.yaml` (2-space indent, line_width 80)
- Preserve `// vim:set ts=8 sts=4 sw=4 tw=0 et:` modelines in C sources (Makefile uses `noet`)
- MSVC: `/utf-8`, `_CRT_SECURE_NO_WARNINGS`; testdir executables link `MultiThreadedDebug` (MSVC) / `ucrtbased` (MINGW)

## Install

```console
cmake --install build
```

- Components: `core` (binary + generated header) and `dict` (dictionaries into `${CMAKE_INSTALL_DATADIR}/cmigemo/<encoding>/`)

## CI

- `.github/workflows/`: builds, tests, profiles and packages on Linux (x86_64 + aarch64), macOS, Windows UCRT64, MSVC and MSYS2 - changes must stay portable across all five

## Constraints

- Default build downloads the dictionary; offline builds need `-DBUILD_DICT=OFF`
- `BUILD_TESTING=OFF` disables tests at configure time
- `profile` target only works with GCC/Clang + `gprof`; skipped otherwise
