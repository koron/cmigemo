---
type: Directory Index
title: "Test Index"
description: "Holds the C test programs and fixture dictionary for cmigemo's CTest suite"
tags: [index]
---

# Overview
Contains the executable C tests for the migemo library (migemo queries, romaji conversion, and rxgen regexp generation) plus their shared test configuration and fixture dictionary, registered with CMake via `add_migemo_test`.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | File | Defines the CTest test targets and the `check` convenience target for all test executables. |
| [migemo.c](./migemo.c) | File | Test program that asserts migemo_query results against expected conversions using the fixture dictionary. |
| [romaji.c](./romaji.c) | File | Test program that verifies romaji-to-hiragana conversion output against expected wordlists. |
| [rxgen.c](./rxgen.c) | File | Test program that checks regexp generation, including escape-character handling, in rxgen. |
| [test_common.h](./test_common.h) | Header | Provides default test-directory macros such as the romaji dictionary directory path. |
| [_testdata/](./_testdata) | Directory | Contains the `migemo-dict` fixture dictionary consumed by the migemo test. |

# References
- [Parent Directory](../index.md)
