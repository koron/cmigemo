---
type: Directory Index
title: "test Index"
description: "Unit test sources and build configuration for the migemo, romaji, and rxgen libraries."
tags: [index]
---

# Overview
This directory contains the C unit test programs (`migemo.c`, `romaji.c`, `rxgen.c`) and their CMake build rules. Each test source is compiled into a CTest executable (`test_<name>_test`) and exercises the public API of the corresponding library against a small static dictionary fixture in `_testdata/`.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [migemo.c](./migemo.c) | Source | Unit test that asserts `migemo_query()` expands romaji and kana input into the expected regular expressions. |
| [romaji.c](./romaji.c) | Source | Unit test that verifies the romaji-to-kana conversion produces the expected word lists. |
| [rxgen.c](./rxgen.c) | Source | Unit test that checks `rxgen` regex generator output including default and custom escape characters. |
| [test_common.h](./test_common.h) | Header | Shared header defining the `TEST_DICTDIR_ROMA2HIRA` default path used by the test programs. |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake script that registers each test source via `add_migemo_test()` as a CTest target. |
| [_testdata/](./_testdata/) | Directory | Static test fixture directory containing the `migemo-dict` dictionary used by the tests. |

# References
- [Parent Directory](../index.md)
