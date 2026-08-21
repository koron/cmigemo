---
type: Directory Index
title: "dict Index"
description: "Dictionary sources for C/Migemo: build script, Perl conversion/optimization tools, static Japanese conversion tables, and a Chinese dictionary."
tags: [index]
---

# Overview
This directory holds the dictionary sources used by C/Migemo: the CMake build that downloads SKK-JISYO.L and produces the `migemo-dict` files per encoding, Perl scripts that convert and optimize the dictionary, the static `.dat` tables for Japanese character conversions, and the bundled Chinese dictionary.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake script that downloads SKK-JISYO.L and builds the `migemo-dict` files for the utf-8, euc-jp, and cp932 encodings. |
| [han2zen.dat](./han2zen.dat) | Data | Half-width to full-width character conversion table. |
| [hira2kata.dat](./hira2kata.dat) | Data | Hiragana to Katakana conversion table. |
| [migemo-dict-zh](./migemo-dict-zh) | Data | Chinese dictionary mapping pinyin syllables to Chinese characters. |
| [optimize-dict.pl](./optimize-dict.pl) | Script | Perl script that rebalances dictionary entries in DFS midpoint order so C/Migemo can load the dictionary efficiently. |
| [roma2hira.dat](./roma2hira.dat) | Data | Table to convert Japanese ROMAJI to HIRAGANA. |
| [skk2migemo.pl](./skk2migemo.pl) | Script | Perl script that converts SKK-JISYO data into tab-separated migemo-dict lines. |
| [zen2han.dat](./zen2han.dat) | Data | Full-width to half-width character conversion table. |

# References
- [Parent Directory](../index.md)
