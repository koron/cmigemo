---
type: Directory Index
title: "tools Index"
description: "Contains the source programs for the cmigemo, romaji, and gen_sdict command-line tools and their CMake build rules."
tags: [index]
---

# Overview
Holds the sources for the command-line tools built by the project (`cmigemo`, `romaji`, `gen_sdict`) along with the CMake rules that define, link, and install them.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | Defines and installs the `cmigemo`, `romaji`, and `gen_sdict` executables with their dictionary-directory compile definitions. |
| [cmigemo.c](./cmigemo.c) | Source Code | Command-line driver that loads a migemo dictionary, accepts query options, and prints regexp expansion results. |
| [gen_sdict.c](./gen_sdict.c) | Source Code | Utility that loads a migemo-dict, converts it to the static dictionary (sdict) format, and saves the result. |
| [romaji.c](./romaji.c) | Source Code | Interactive romaji-to-hiragana/katakana/hankaku/zenkaku conversion console. |

# References
- [Parent Directory](../index.md)
