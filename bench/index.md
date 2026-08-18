---
type: Directory Index
title: "bench Index"
description: "Benchmark and profiling programs that measure migemo dictionary load and single-character query speed."
tags: [index]
---

# Overview

This directory contains the profiling build of the migemo library and a benchmark program that repeatedly queries single characters to measure query performance, producing a `profile.log` via gprof.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules that build the profiling library, the profile_char_query executable, and the profile custom target. |
| [copy_gmon.cmake](./copy_gmon.cmake) | Build Support | CMake script that copies the qspeed gmon output file to gmon.out for gprof. |
| [profile_char_query.c](./profile_char_query.c) | Source | Benchmark program that times dictionary loading and single-character migemo queries. |

# References
- [Parent Directory](../index.md)
