---
type: Directory Index
title: "Bench Index"
description: "Benchmark and profiling tools that measure migemo dictionary load and query performance via gprof."
tags: [index]
---

# Overview
This directory contains C benchmark programs (`char_query`, `migemo_open`) that time `migemo_open` dictionary loading and character queries, plus CMake rules that build them with gprof instrumentation and produce `profile_*.log` reports.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | CMake rules building the profiled migemo static library and per-benchmark `bench_*`/`profile_*` targets. |
| [char_query.c](./char_query.c) | Source Code | Benchmark that profiles single-character `migemo_query` lookups across the alphabet. |
| [migemo_open.c](./migemo_open.c) | Source Code | Benchmark that profiles repeated `migemo_open`/`migemo_close` dictionary load cycles. |

# References
- [Parent Directory](../index.md)
