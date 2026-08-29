---
type: Directory Index
title: "tools Index"
description: "Contains the source programs for the cmigemo and romaji command-line tools and their CMake build rules."
tags: [index]
---

# Overview
Holds the sources for the command-line tools built by the project (`cmigemo`, `romaji`) along with the CMake rules that define, link, and install them.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | Defines and installs the `cmigemo` and `romaji` executables with their dictionary-directory compile definitions. |
| [cmigemo.c](./cmigemo.c) | Source Code | Command-line driver that loads a migemo dictionary, accepts query options, and prints regexp expansion results. |
| [romaji.c](./romaji.c) | Source Code | Interactive romaji-to-hiragana/katakana/hankaku/zenkaku conversion console. |

# References
- [Parent Directory](../index.md)
