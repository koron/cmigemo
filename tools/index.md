---
type: Directory Index
title: "tools Index"
description: "Command-line tools (cmigemo and romaji) that exercise the migemo library, built and installed by CMake."
tags: [index]
---

# Overview

This directory contains the command-line driver programs of the project: the `cmigemo` migemo query driver and the `romaji` romaji-conversion console, along with the CMake rules that build and install them.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules that build the cmigemo and romaji executables, set dictionary paths, and install cmigemo. |
| [cmigemo.c](./cmigemo.c) | Source | Main driver of the cmigemo executable that interactively queries the migemo library from standard input. |
| [romaji.c](./romaji.c) | Source | Console tool that demonstrates romaji conversion through hira2kata, han2zen, and zen2han sub-dictionaries. |

# References
- [Parent Directory](../index.md)
