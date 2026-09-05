# AGENTS.md

## Requirements

- **Build**: CMake (3.21+), C11 compiler (GCC, Clang, MSVC)
- **Dictionary generation** (default, `BUILD_DICT=ON`): `curl`, `iconv`, `gzip`, `perl` and network access (downloads `SKK-JISYO.L`)
- **Profiling** (GCC/Clang only): `gprof`

## Build

- Configure: `cmake -B build` (default type: `Release`; for debug: `-DCMAKE_BUILD_TYPE=Debug`)
- Build: `cmake --build build --parallel` (default build also builds all dictionaries + sdict; offline: `-DBUILD_DICT=OFF`, and `-DBUILD_SDICT=OFF` to skip only sdict)
- Artifacts: executables in `build/bin/` (`cmigemo`, `romaji`, `gen_sdict`, `test_*_test`, `bench_*`), libraries in `build/lib/` (`migemo` shared, `migemo_static`, `migemo_profile`)
- Test: `cmake --build build --target test` (CTest) or `--target check` (ctest with `--output-on-failure`, defined in `test/CMakeLists.txt`)
- Profiling: `cmake --build build --target profile` (needs `gprof`; GCC/Clang only, not MSVC) -> `build/bench/profile_*.log`
- Package: `cmake --build build --target package` (CPack ZIP on Windows, TGZ elsewhere; produces two archives: main + `-sdict` suffix)
- Clean: `cmake --build build --target clean` or `rm -rf build`

## Makefile Shortcuts

`make build` (configure + build), `make test` (configure + build + `check`), `make profile`, `make package` (Release), `make format` (clang-format on `src test include bench tools`; temporarily renames `include/migemo.h.in` to `migemo.h` so it gets formatted), `make format-cmake` (cmakefmt), `make tags`, `make clean`, `make distclean`

## Run CLI

```console
./build/bin/cmigemo -d build/dict/mdict/utf-8/migemo-dict
```

- Dictionary artifacts land in `build/dict/mdict/<enc>/` (sdict in `build/dict/sdict/<enc>/`). `cmigemo`'s default search order: `./dict/mdict/<enc>/`, `../dict/mdict/<enc>/`, `./build/dict/mdict/<enc>/`, then legacy `./dict/`, `../dict/`, `./build/dict/`, then the install path (`CMAKE_INSTALL_DATADIR/cmigemo/<enc>`, baked in via `CMIGEMO_DICTDIR`) - so running from the repo root works after a default build.
- (Windows: `cmigemo.exe`; default dict encoding is `cp932` there, `utf-8` elsewhere.)
- Options: `-d <dict>`, `-s`/`-u <subdict>` (max 8, `-u` is an alias), `--convert` (switch to static dictionary, sdict), `--save <dict>` (implies `--convert`, saves sdict), `--sdict <dict>` (load sdict; conflicts with `-d/-s/-u` and `--convert/--save`), `-w <word>` (expand and exit), `-v`/`-e` (vim/emacs regexp style), `-n` (no newline match), `-q`
- `migemo_open()` auto-loads `roma2hira.dat`, `hira2kata.dat`, `han2zen.dat`, `zen2han.dat` from the dictionary's directory; **missing sub-dictionaries are silently ignored** (src/migemo.c)

## Dictionary Generation

- Pipeline: download `SKK-JISYO.L` (EUC-JISX0213) from `https://skk-dev.github.io/dict/` -> `iconv` to UTF-8 -> `dict/skk2migemo.pl` -> `dict/optimize-dict.pl` -> per-encoding copies
- Output: base `build/dict/migemo-dict`, text dicts `build/dict/mdict/<enc>/`, static dicts `build/dict/sdict/<enc>/migemo-sdict`; encodings: `utf-8`, `euc-jp`, `cp932`
- `migemo-dict-zh` (Chinese dict) is copied only into `utf-8`; cp932 conversion tolerates iconv errors
- Targets: `dictionaries` (all mdict), `mdict_<enc>`, `sdict` / `sdict_<enc>` (only with `BUILD_SDICT=ON`, default follows `BUILD_DICT`)
- Skip all dictionary work (and the curl/iconv/gzip/perl requirements): `cmake -B build -DBUILD_DICT=OFF`

## Test

- Executables `test_migemo_test`, `test_romaji_test`, `test_rxgen_test` from `test/migemo.c`, `test/romaji.c`, `test/rxgen.c`; CTest names `test_migemo`, `test_romaji`, `test_rxgen`
- Single test: run `build/bin/test_<name>_test` directly or `ctest -R test_<name>` (from `build/`)
- Tests use static fixtures (`test/_testdata/migemo-dict`, `dict/roma2hira.dat` via `TEST_DICTDIR_*`), so they run offline without the downloaded dictionary
- New tests: add `test/<name>.c` + `add_migemo_test(<name> <name>.c)` in `test/CMakeLists.txt` (optionally pass `TEST_DICTDIR_*` compile definitions as ARGN)

## Benchmark / Profiling

- `bench/char_query.c`, `bench/migemo_open.c` -> `bench_<name>` executables, linked against `migemo_profile` with `-pg` (GNU/Clang + gprof only); `DICTDIR` compile definition points at `build/dict/mdict/utf-8`
- `cmake --build build --target profile` runs `profile_<name>` for each bench -> `build/bench/profile_<name>.log`
- New benchmark: add `bench/<name>.c` + `add_profile_target(<name>)` in `bench/CMakeLists.txt` (marked with a comment)
- Bench/profile targets only exist when `BUILD_TESTING=ON` (default)

## Project Structure

- OKF Indexing: root and `src`, `dict`, `test`, `tools`, `bench`, `wasm` contain an `index.md` (Open Knowledge Format) acting as a directory map. Read the relevant `index.md` first to locate files before broad searches.
- `include/migemo.h.in` - public C API template; generated to `build/src/include/migemo.h` at configure time; version comes from `project(VERSION)` in top-level `CMakeLists.txt`
- `src/` - core: `migemo_objects` objects -> `migemo` (shared, `migemo.def`/`migemo.rc` exports) and `migemo_static`; `stree.c` = static dictionary (sdict) binary format
- `tools/cmigemo.c` - CLI (`cmigemo`, links shared lib); `tools/romaji.c` - interactive romaji console (`romaji`); `tools/gen_sdict.c` - mdict->sdict converter used by dict build
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
cmake --install build [--component core|mdict|sdict]
```

Dictionaries go to `${CMAKE_INSTALL_DATADIR}/cmigemo/<encoding>/`. (README's `--component dict` is stale; the components are `core`, `mdict`, `sdict`.)

## CI

- `.github/workflows/ci.yml` invokes reusable workflows: Linux (x86_64 + aarch64), macOS, Windows UCRT64/MSVC/MSYS2, WebAssembly; each job builds and tests (`--target test`), profiles (only Linux/UCRT64/MSYS2; never on macOS/MSVC/WASM) and packages; changes must stay portable across all
- The sdict package is uploaded only from the Linux x86_64 job
- Tags (`refs/tags/*`) trigger a draft GitHub Release with all packages; tags containing `-alpha.`/`-beta.` are marked prerelease
- WASM job: `emcmake cmake -B build-wasm -DBUILD_DICT=ON`, then `node wasm/test/test.js`; packages `migemo_wasm.js/.wasm` + `wasm/` assets

## Constraints

- Default build downloads the dictionary; offline builds need `-DBUILD_DICT=OFF`
- `BUILD_TESTING=OFF` disables tests and the bench/profile targets at configure time
- `profile` target requires `gprof` (GCC/Clang); MSVC builds have no profiling step (CI never runs it there)
