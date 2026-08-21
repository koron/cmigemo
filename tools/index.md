---
type: Directory Index
title: "tools Index"
description: "Console driver tools and their CMake build rules for the cmigemo project."
tags: [index]
---

# Overview
This directory holds standalone console tools (the `cmigemo` query driver and the `romaji` romaji-conversion console) together with the CMake rules that build and install them.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build Script | Defines CMake targets that build the `cmigemo` and `romaji` executables and install `cmigemo` into the binary directory. |
| [cmigemo.c](./cmigemo.c) | Source | CLI driver that opens a migemo dictionary and runs interactive regex queries with vim/emacs regexp options. |
| [romaji.c](./romaji.c) | Source | Romaji conversion console tool that interactively expands queries through romaji-to-hiragana, hiragana-to-katakana, and kana conversion tables. |

# References
- [Parent Directory](../index.md)
