---
type: Directory Index
title: "test Index"
description: "CTest test suite (test1) with C test sources verifying the migemo library and its fixtures."
tags: [index]
---

# Overview

This directory contains the executable-based test suite for the cmigemo library: a CMake target that builds the `test1` binary from multiple C test files, registered with CTest, and run against a static migemo-dict fixture.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules that build the `test1` executable and register the CTest suite. |
| [main.c](./main.c) | Source | Test runner entry point that invokes the individual test functions. |
| [test1.c](./test1.c) | Source | Main library test that loads the dictionary fixture and checks migemo conversion. |
| [test_rxgen.c](./test_rxgen.c) | Source | Test of the regular expression generator (rxgen) module. |
| [test1/](./test1/) | Directory Index | Test fixture directory holding the static `migemo-dict` file used by the tests. |

# References
- [Parent Directory](../index.md)
