---
type: Directory Index
title: "bench Index"
description: "Benchmark and profiling tools for measuring migemo dictionary load and query performance."
tags: [index]
---

# Overview
Contains CMake build rules and C benchmark programs for profiling the migemo library's dictionary loading (`migemo_open`) and single-character query performance, with `gprof` integration for call-graph analysis.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | Defines the `migemo_profile` static library and `profile_char_query`/`profile_migemo_open` executables with gprof instrumentation. |
| [copy_gmon.cmake](./copy_gmon.cmake) | Build Script | CMake policy script that copies `qspeed.gmon` to `gmon.out` for gprof consumption. |
| [profile_char_query.c](./profile_char_query.c) | Benchmark | Measures the time for single-character `migemo_query` calls across the alphabet over multiple trials. |
| [profile_migemo_open.c](./profile_migemo_open.c) | Benchmark | Measures the time for repeated `migemo_open`/`migemo_close` dictionary load cycles. |

# References
- [Parent Directory](../index.md)
