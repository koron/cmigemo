---
type: Directory Index
title: "src Index"
description: "C sources and headers for the migemo shared library core (dictionary tree, romaji, regexp, charset), with CMake build rules and Windows build support files."
tags: [index]
---

# Overview

This directory holds the C implementation of the `migemo` shared library: the migemo tree (dictionary storage/query), romaji conversion, regular expression generation, character set/encoding handling, and supporting utilities (string buffer, word list, filename helpers), plus the CMake build script and Windows module-definition/resource files.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [migemo.c](./migemo.c) | Source | Core implementation of the public migemo API (open/close/query and dictionary loading). |
| [migemo_struct.h](./migemo_struct.h) | Header | Defines the internal `struct migemo` aggregating the mtree, romaji tables, and rxgen state. |
| [mtree.c](./mtree.c) | Source | Migemo tree (mtree) operations for loading the dictionary and querying words. |
| [mtree.h](./mtree.h) | Header | Public declarations of the migemo tree (mtree) operations. |
| [romaji.c](./romaji.c) | Source | Romaji conversion support for input handling. |
| [romaji.h](./romaji.h) | Header | Public declarations of the romaji conversion module. |
| [rxgen.c](./rxgen.c) | Source | Regular expression generator producing patterns for matching input against the dictionary. |
| [rxgen.h](./rxgen.h) | Header | Public declarations of the regular expression generator. |
| [charset.c](./charset.c) | Source | Character set and multi-byte encoding conversion helpers (CP932, EUC-JP, UTF-8). |
| [charset.h](./charset.h) | Header | Public declarations of the character set conversion module. |
| [strbuf.c](./strbuf.c) | Source | Dynamic string buffer implementation used throughout the library. |
| [strbuf.h](./strbuf.h) | Header | Public declarations of the string buffer module. |
| [trie.c](./trie.c) | Source | Trie statistics printing used for diagnosing the dictionary tree structure. |
| [trie.h](./trie.h) | Header | Public declarations of the trie statistics helpers. |
| [wordlist.c](./wordlist.c) | Source | Word list handling for dictionary expansion. |
| [wordlist.h](./wordlist.h) | Header | Public declarations of the word list module. |
| [filename.c](./filename.c) | Source | Filename manipulation helpers for locating dictionaries. |
| [filename.h](./filename.h) | Header | Public declarations of the filename helper module. |
| [common.h](./common.h) | Header | Shared code (e.g., debugging facilities) injected into all .c files. |
| [migemo.def](./migemo.def) | Build Support | MSVC module definition file controlling exported library symbols. |
| [migemo.rc.in](./migemo.rc.in) | Build Support | CMake template generating the Windows resource file for the library. |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules building the migemo library, generating the public header, and configuring Windows resources. |

# References
- [Parent Directory](../index.md)
