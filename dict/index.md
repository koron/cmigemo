---
type: Directory Index
title: "dict Index"
description: "Dictionary build scripts and static .dat conversion tables used to generate the migemo-dict dictionaries."
tags: [index]
---

# Overview

This directory holds the dictionary build pipeline for cmigemo: Perl scripts that convert the SKK-JISYO Japanese dictionary into migemo format and optimize it, the static sub-dictionaries (Romaji/hiragana conversion, character width conversions), and the CMake rules that orchestrate the download-and-build process.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | CMake rules that download SKK-JISYO.L and run the conversion scripts to build the per-encoding dictionaries. |
| [skk2migemo.pl](./skk2migemo.pl) | Script | Perl script that converts the SKK-JISYO dictionary into migemo-dict format. |
| [optimize-dict.pl](./optimize-dict.pl) | Script | Perl script that optimizes migemo-dict so the C/Migemo library can load it. |
| [roma2hira.dat](./roma2hira.dat) | Data | Table to convert Romaji input to HIRAGANA in Japanese. |
| [hira2kata.dat](./hira2kata.dat) | Data | HIRAGANA-to-Katakana (平仮名→カタカナ) conversion table. |
| [han2zen.dat](./han2zen.dat) | Data | Half-width-to-full-width (半角→全角) character conversion table. |
| [zen2han.dat](./zen2han.dat) | Data | Full-width-to-half-width (全角→半角) character conversion table. |
| [migemo-dict-zh](./migemo-dict-zh) | Data | Chinese (zh) migemo dictionary copied into the utf-8 dictionary build. |

# References
- [Parent Directory](../index.md)
