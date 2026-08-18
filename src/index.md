---
type: Directory Index
title: "src Index"
description: "Source files for the cmigemo core shared library (migemo) and its CLI tool, plus auxiliary build/test tools."
tags: [index]
---

# Overview

This directory contains the C sources, headers, and CMake build scripts for the `migemo` shared library and the `cmigemo` CLI executable, along with the `testdir` folder holding auxiliary tools (`romaji`, `qspeed`) that are built outside the core library.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [migemo.c](./migemo.c) | Source | Core migemo public API implementation (library and dictionary loading). |
| [migemo_struct.h](./migemo_struct.h) | Header | Internal struct definitions shared across the core library. |
| [rxgen.c](./rxgen.c) | Source | Regular expression generator for matching input against the migemo dictionary. |
| [rxgen.h](./rxgen.h) | Header | Public declarations of the regular expression generator. |
| [romaji.c](./romaji.c) | Source | Romaji (romajikana) conversion support for input handling. |
| [romaji.h](./romaji.h) | Header | Public declarations of the romaji conversion module. |
| [charset.c](./charset.c) | Source | Character set and multi-byte encoding conversion helpers. |
| [charset.h](./charset.h) | Header | Public declarations of the character set conversion module. |
| [strbuf.c](./strbuf.c) | Source | Dynamic string buffer implementation used throughout the library. |
| [strbuf.h](./strbuf.h) | Header | Public declarations of the string buffer module. |
| [trie.c](./trie.c) | Source | Trie data structure used for dictionary storage and lookup. |
| [trie.h](./trie.h) | Header | Public declarations of the trie data structure. |
| [mtree.c](./mtree.c) | Source | Migemo tree (mtree) operations for building and searching the trie. |
| [mtree.h](./mtree.h) | Header | Public declarations of the migemo tree operations. |
| [wordlist.c](./wordlist.c) | Source | Word list handling for dictionary expansion. |
| [wordlist.h](./wordlist.h) | Header | Public declarations of the word list module. |
| [filename.c](./filename.c) | Source | Filename manipulation helpers for locating dictionaries. |
| [filename.h](./filename.h) | Header | Public declarations of the filename helper module. |
| [common.h](./common.h) | Header | Shared code (e.g., debugging facilities) injected into all .c files. |
| [migemo.def](./migemo.def) | Build Support | MSVC module definition file controlling exported library symbols. |
| [migemo.rc.in](./migemo.rc.in) | Build Support | CMake template generating the Windows resource file for the library. |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake build rules for the migemo library, cmigemo binary, and tools. |

# References
- [Parent Directory](../index.md)
