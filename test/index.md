---
type: Directory Index
title: "test Index"
description: "CTest test suite (test1) for the cmigemo migemo library, with C sources, CMake build rules, and a static dictionary fixture."
tags: [index]
---

# Overview

This directory contains the executable-based CTest suite for the cmigemo library. The `test1` binary is built from `main.c`, `test1.c`, and `test_rxgen.c`, verifies migemo query conversion and regular-expression generation behavior, and runs against the static `test1/migemo-dict` fixture.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules that build the `test1` executable, register the `test_migemo1` CTest, and define the `check` target. |
| [main.c](./main.c) | Source | Test runner entry point that invokes the test1 and test_rxgen test functions. |
| [test1.c](./test1.c) | Source | Main library test that loads the migemo-dict and roma2hira fixtures and checks migemo_query output. |
| [test_rxgen.c](./test_rxgen.c) | Source | Tests the rxgen regular-expression generator escape-character behavior and the migemo_set_escape_chars API. |
| [test1/](./test1/) | Directory | Test fixture directory holding the static `migemo-dict` wordlist file. |

# References
- [Parent Directory](../index.md)
