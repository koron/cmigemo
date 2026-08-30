# AGENTS.md

## Requirements

- **Build**: CMake (3.21+), C11 compiler (GCC, Clang, MSVC)
- **Dictionary generation** (default, `BUILD_DICT=ON`): `curl`, `iconv`, `gzip`, `perl` and network access (downloads `SKK-JISYO.L`)
- **Profiling** (GCC/Clang only): `gprof`

## Build

- Configure: `cmake -B build` (default type: `Release`; for debug: `-DCMAKE_BUILD_TYPE=Debug`)
- Build: `cmake --build build --parallel`
- Artifacts: executables in `build/bin/` (`cmigemo`, `romaji`, `test_*_test`, `bench_char_query`, `bench_migemo_open`), libraries in `build/lib/` (`migemo` shared, `migemo_static`, `migemo_profile`)
- Test: `cmake --build build --target test` (CTest) or `--target check` (ctest with `--output-on-failure`, defined in `test/CMakeLists.txt`)
- Profiling: `cmake --build build --target profile` (needs `gprof`; GCC/Clang only, not MSVC) -> `build/bench/profile_*.log`
- Package: `cmake --build build --target package` (CPack ZIP on Windows, TGZ elsewhere)
- Clean: `cmake --build build --target clean` or `rm -rf build`

## Makefile Shortcuts

`make build`, `make test` (build + `check`), `make profile`, `make package`, `make format` (clang-format on `src test include bench tools`), `make format-cmake` (cmakefmt), `make tags`, `make clean`, `make distclean`

## Run CLI

```console
./build/bin/cmigemo -d build/dict/utf-8/migemo-dict
```

(Windows: `cmigemo.exe`; default dict encoding is `cp932` there, `utf-8` elsewhere.)

- Options: `-d <dict>`, `-s`/`-u <subdict>` (max 8, `-u` is an alias), `--convert` (switch to static dictionary, sdict), `--save <dict>` (imply `--convert`, save sdict), `--sdict <dict>` (load sdict; conflicts with `-d/-s/-u` and `--convert/--save`), `-w <word>` (expand and exit), `-v`/`-e` (vim/emacs regexp style), `-n` (no newline match), `-q`.
- Without `-d`, the binary searches (platform-dependent): `./dict/<enc>/`, `../dict/<enc>/`, `./build/dict/<enc>/`, then `./dict/`, `../dict/`, `./build/dict/`, plus the compiled-in install dir (`<DATADIR>/cmigemo/<enc>`).
- `migemo_open()` auto-loads `roma2hira.dat`, `hira2kata.dat`, `han2zen.dat`, `zen2han.dat` from the dictionary's directory; **missing sub-dictionaries are silently ignored** (src/migemo.c).

## Dictionary Generation

- Pipeline: download `SKK-JISYO.L` (EUC-JISX0213) from `https://skk-dev.github.io/dict/` -> `iconv` to UTF-8 -> `dict/skk2migemo.pl` -> `dict/optimize-dict.pl` -> per-encoding copies
- Encodings: `utf-8`, `euc-jp`, `cp932`, each built into `build/dict/<encoding>/`; default: Windows = `cp932`, non-Windows = `utf-8`
- `migemo-dict-zh` (Chinese dict) is copied only into `utf-8`; cp932 conversion tolerates iconv errors
- Skip all dictionary work (and the curl/iconv/gzip/perl requirements): `cmake -B build -DBUILD_DICT=OFF`
- Build only dictionaries: `cmake --build build --target dictionaries` (per-encoding `dict_<encoding>` targets also exist)
- Build static dictionaries (sdict): `cmake --build build --target sdict` (per-encoding `sdict_<encoding>` targets also exist)

## Test

- Executables `test_migemo_test`, `test_romaji_test`, `test_rxgen_test` from `test/migemo.c`, `test/romaji.c`, `test/rxgen.c`; CTest names `test_migemo`, `test_romaji`, `test_rxgen`
- New tests: add `test/<name>.c` and an `add_migemo_test(<name> <name>.c)` call in `test/CMakeLists.txt` (optionally pass `TEST_DICTDIR_*` compile definitions as ARGN)
- Test fixture: `test/_testdata/migemo-dict` (static file, not generated); tests explicitly load `dict/roma2hira.dat` via `TEST_DICTDIR_ROMA2HIRA`
- Run all: `cmake --build build --target check`

## Benchmark / Profiling

- `bench/char_query.c`, `bench/migemo_open.c` -> `bench_char_query`, `bench_migemo_open` executables; run against the utf-8 dict (`DICTDIR=build/dict/utf-8`)
- `cmake --build build --target profile` runs all benchmarks and gprof's them -> `build/bench/profile_<name>.log`
- New benchmark: add `bench/<name>.c` + `add_profile_target(<name>)` in `bench/CMakeLists.txt` (marked with a comment)
- Profiling only works with GCC/Clang + `gprof`; the profile targets invoke gprof unconditionally and are broken under MSVC
- Bench/profile targets only exist when `BUILD_TESTING=ON` (default)

## Project Structure

- OKF Indexing: Each main directory contains an `index.md` (Open Knowledge Format) acting as a directory map. ALWAYS read the relevant `index.md` first to locate source files efficiently before performing broad searches.
- `include/migemo.h.in` - public C API template; generated to `build/src/include/migemo.h` at configure time; version comes from `project(VERSION)` in top-level `CMakeLists.txt`
- `src/` - core shared library: `migemo_objects` objects -> `migemo` (shared, with `.def`/`.rc` exports) and `migemo_static`; `stree.c` = static dictionary (sdict) binary format
- `tools/cmigemo.c` - CLI executable (`cmigemo`); `tools/romaji.c` - interactive romaji conversion console (`romaji`, links `migemo_static`)
- `bench/` - benchmark and gprof profiling harness
- `dict/` - dictionary build scripts + static `.dat` sub-dictionaries + `migemo-dict-zh`
- `wasm/` - Emscripten port; built only under `EMSCRIPTEN` (`emcmake cmake -B build-wasm -DBUILD_DICT=ON`); JS API in `wasm/index.js`, browser test via `node wasm/test/test.js`
- `misc/` - reference C# wrapper and vim plugin (not built)
- `doc/` - Japanese docs + doxygen config

## Coding Standards

- C formatting: `clang-format` per `.clang-format` (LLVM-based, 4-space indent, 80 cols, return type on own line); `make format`
- CMake formatting: `cmakefmt` per `.cmakefmt.yaml` (2-space indent, line_width 80); `make format-cmake`
- Preserve `// vim:set ts=8 sts=4 sw=4 tw=0 et:` modelines in C sources (Makefile uses `noet`)
- MSVC: `/utf-8 /W3 /wd4710 /wd4711 /wd5045`, `_CRT_SECURE_NO_WARNINGS`

## Install

```console
cmake --install build
```

- Components: `core` (binary + generated header) and `dict` (dictionaries into `${CMAKE_INSTALL_DATADIR}/cmigemo/<encoding>/`)

## CI

- `.github/workflows/ci.yml` fans out to Linux (x86_64 + aarch64), macOS, Windows UCRT64, MSVC, MSYS2 and WebAssembly; each job builds, tests, profiles (gprof on MSYS2/Linux/macOS only) and packages; changes must stay portable across all
- Tags (`refs/tags/*`) trigger a draft GitHub Release with all uploaded packages
- WASM job: `emcmake cmake -B build-wasm -DBUILD_DICT=ON`, then `node wasm/test/test.js`; packages `migemo_wasm.js/.wasm` + `wasm/` assets

## Constraints

- Default build downloads the dictionary; offline builds need `-DBUILD_DICT=OFF`
- `BUILD_TESTING=OFF` disables tests and the bench/profile targets at configure time
- `profile` target requires `gprof` (GCC/Clang); MSVC builds have no profiling step (CI never runs it there)
