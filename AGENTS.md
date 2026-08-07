# AGENTS.md

## Build System & Requirements

- **Requirements**:
  - Build: CMake (3.16+), C11 compiler (gcc, clang, msvc)
  - Dictionary Generation: `curl`, `iconv`, `gzip`, `perl`
  - Profiling: `gperf`
- **Configure**: `cmake -B build` (Default build type is `Release`. For Debug: `cmake -B build -DCMAKE_BUILD_TYPE=Debug`)
- **Build**: `cmake --build build --parallel`
- **Test**: `cmake --build build --target test`
- **Profiling**: `cmake --build build --target profile` (generates `build/profile.log`)
- **Clean**: `cmake --build build --target clean` or `rm -rf build`

## How to Run CLI Tool

Build first, then execute with a generated dictionary:

```console
./build/src/cmigemo -d build/dict/utf-8/migemo-dict
```

Pass `-d <dict>` or `-s <subdict>` to specify custom dictionary paths.

## Dictionary Generation

* **Process**: Downloads `SKK-JISYO.L`, converts to UTF-8, and runs `dict/skk2migemo.pl` / `dict/optimize-dict.pl` to build base dictionary.
* **Encodings**: Generates dictionaries for `utf-8`, `euc-jp`, and `cp932` under `build/dict/<encoding>/`.
* **Default Encoding Target**:
* Windows: `cp932`
* Non-Windows: `utf-8`

## Project Structure

* `include/`: Public C API headers (`migemo.h`).
* `src/`: Core library implementation (`migemo.c`, `rxgen.c`, `romaji.c`, etc.) and CLI executable (`main.c`).
* `dict/`: Dictionary build scripts (`skk2migemo.pl`, `optimize-dict.pl`) and CMake targets.
* `test/`: Unit tests (`test1`) and benchmarking/profiling utilities.

## Coding Standards & Compiler Quirks

* **Code Formatting**:
* C/C++ files: LLVM style via `clang-format -i <file>`
* CMake files: Configured via `.cmakefmt.yaml` (2-space indent, column limit 80)

* **Editor Modelines**: Preserve editor headers in source files (e.g., `// vim:set ts=8 sts=4 sw=4 tw=0 et:`).
* **MSVC Rules**: Requires `/utf-8` and `_CRT_SECURE_NO_WARNINGS`.

## Install

```console
cmake --install build
```

Installs binaries/headers to standard system paths and dictionaries to `${CMAKE_INSTALL_DATADIR}/cmigemo/<encoding>/`.
