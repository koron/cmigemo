---
type: Directory Index
title: "cmigemo Index"
description: "C/Migemo regexp generator library: a CMake C11 project bundling the migemo shared library, dictionary sources, CTest suite, benchmarks, console tools, and a WebAssembly port."
tags: [index]
---

# Overview
C/Migemo is a C-language implementation of Migemo, a regexp pattern generator/expander that converts Romaji input into regular expressions for incremental Japanese text searches. This root directory contains the CMake build, the shared library sources, dictionary sources, the CTest suite, benchmarking, console driver tools, and a WebAssembly port.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | Top-level CMake build definition for the C/Migemo project. |
| [Makefile](./Makefile) | Build Script | Convenience make targets wrapping the CMake build (build, test, profile, package). |
| [README.md](./README.md) | Documentation | Project overview and usage documentation for the C/Migemo library. |
| [AGENTS.md](./AGENTS.md) | Agent Guide | AI-agent guidance covering build, test, dictionary, and project conventions. |
| [LICENSE](./LICENSE) | Legal | License text for C/Migemo. |
| [.clang-format](./.clang-format) | Config | clang-format configuration for the C sources. |
| [.cmakefmt.yaml](./.cmakefmt.yaml) | Config | cmakefmt configuration for CMake formatting. |
| [.gitignore](./.gitignore) | Config | Git ignore rules for build outputs and generated files. |
| [src/](./src/index.md) | Directory Index | C sources and headers for the migemo shared library core (dictionary tree, static tree, romaji, regexp generation, charset), with CMake build rules and Windows build support files. |
| [dict/](./dict/index.md) | Directory Index | Dictionary sources for C/Migemo: build script, Perl conversion/optimization tools, static Japanese conversion tables, and a Chinese dictionary. |
| [test/](./test/index.md) | Directory Index | Unit test sources and build configuration for the migemo, romaji, and rxgen libraries. |
| [bench/](./bench/index.md) | Directory Index | Benchmarks and profiling harnesses for measuring migemo dictionary loading and query performance. |
| [tools/](./tools/index.md) | Directory Index | Contains the source programs for the cmigemo, romaji, and gen_sdict command-line tools and their CMake build rules. |
| [wasm/](./wasm/index.md) | Directory Index | WebAssembly build of C/Migemo with its JavaScript API wrapper, demo page, dictionaries, and test files. |
| [include/](./include/) | Directory | Public C API header template (migemo.h.in) for the migemo library. |
| [doc/](./doc/) | Directory | Japanese README and Doxygen configuration for API documentation generation. |
| [misc/](./misc/) | Directory | Reference C# wrapper and vim plugin (not part of the build). |
| [.github/](./.github/) | Directory | GitHub CI workflows and Dependabot configuration. |

# References
- [Agent Guidelines](./AGENTS.md)
- [Project Readme](./README.md)
