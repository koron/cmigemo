---
type: Directory Index
title: "cmigemo Index"
description: "Repository root of C/Migemo, a C library for generating Japanese regexp patterns from Romaji input."
tags: [index]
---

# Overview

This is the root of the C/Migemo project: a C-language implementation of Migemo that converts Romaji input into regular expressions for incremental Japanese text searching, along with its CMake build system, dictionary generation, benchmarks, tools, tests, and documentation.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [README.md](./README.md) | Document | Project introduction explaining C/Migemo and its build and usage. |
| [AGENTS.md](./AGENTS.md) | Document | Instructions for AI agents covering build, test, and coding conventions. |
| [CMakeLists.txt](./CMakeLists.txt) | Build | Top-level CMake project definition for the cmigemo library, tools, and tests. |
| [Makefile](./Makefile) | Build | Convenience shortcuts wrapping CMake targets such as build, test, and format. |
| [LICENSE](./LICENSE) | Document | License text for the project. |
| [tags](./tags) | Build Support | Generated ctags index for code navigation. |
| [.clang-format](./.clang-format) | Configuration | clang-format style rules for the C sources. |
| [.cmakefmt.yaml](./.cmakefmt.yaml) | Configuration | cmakefmt formatting rules for CMakeLists files. |
| [.gitignore](./.gitignore) | Configuration | Git exclusion rules for generated build output. |
| [src/](./src/index.md) | Directory Index | Source files for the cmigemo core shared library (migemo) and its CLI tool, plus auxiliary build/test tools. |
| [tools/](./tools/) | Directory | CMake build definitions and sources for the cmigemo and romaji command-line tools. |
| [bench/](./bench/) | Directory | Benchmark and profiling programs for character query speed measurement. |
| [test/](./test/index.md) | Directory Index | CTest test suite (test1) with C test sources verifying the migemo library and its fixtures. |
| [dict/](./dict/index.md) | Directory Index | Dictionary build scripts and static .dat conversion tables used to generate the migemo-dict dictionaries. |
| [doc/](./doc/) | Directory | Japanese documentation and Doxygen configuration. |
| [include/](./include/) | Directory | Public header template (migemo.h.in) installed at build time. |
| [misc/](./misc/) | Directory | Reference C# wrapper and a vim plugin, not part of the build. |
| [wasm/](./wasm/index.md) | Directory Index | WebAssembly build of the cmigemo library with emscripten wrappers and a browser test page. |
| [.github/](./.github/) | Directory | GitHub configuration including dependabot and CI workflow definitions. |

# References
- [Agent Guidelines](./AGENTS.md)
- [Project Readme](./README.md)
