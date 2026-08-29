---
type: Directory Index
title: "bench Index"
description: "Benchmarks and profiling harnesses for measuring migemo dictionary loading and query performance."
tags: [index]
---

# Overview
Contains standalone benchmark programs that measure migemo performance (dictionary loading and single-character queries) and the CMake targets that build and run them, including gprof-based profiling for GCC/Clang.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [bench_common.h](./bench_common.h) | C header | Provides cross-platform high-resolution timing utilities (`time_now`, `time_to_sec`, `TIME_MEASURE_ADD`) shared by the benchmark programs. |
| [char_query.c](./char_query.c) | C source | Benchmark that times dictionary loading, single-character queries, and sdict switching, run against the utf-8 dictionary. |
| [CMakeLists.txt](./CMakeLists.txt) | CMake script | Defines the `migemo_profile` static library and the `bench_*`/`profile_*`/`profile` targets that build, run, and gprof the benchmarks. |
| [copy_if_exists.cmake](./copy_if_exists.cmake) | CMake script | Helper script invoked by the profile targets to copy a `.gmon` file into `gmon.out` only if it exists. |
| [migemo_open.c](./migemo_open.c) | C source | Benchmark that repeatedly calls `migemo_open` and `migemo_close` to measure dictionary load and close times. |

# References
- [Parent Directory](../index.md)
