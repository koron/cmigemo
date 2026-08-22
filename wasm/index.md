---
type: Directory Index
title: "wasm Index"
description: "Emscripten WebAssembly build of C/Migemo exposing a browser/Node JavaScript API for dictionary queries, with a demo page, runtime dictionaries, and tests."
tags: [index]
---

# Overview
This directory compiles the C/Migemo engine to WebAssembly via CMake/Emscripten, producing a modular ES module (`createMigemoModule`) plus an `index.js` API wrapper that lets web pages and Node load a dictionary and build query regular expressions.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [.gitignore](./.gitignore) | Text | Ignores the generated `migemo_wasm.js`/`migemo_wasm.wasm` artifacts and the checked-out `dict/` directory. |
| [CMakeLists.txt](./CMakeLists.txt) | CMake | Compiles the migemo C sources into the `migemo_wasm` Emscripten target exporting `createMigemoModule` and copies the generated JS/WASM into this directory. |
| [demo.html](./demo.html) | HTML | Browser demo that loads the module, fetches the dictionaries from `./dict`, and exposes `window.migemo` for console use. |
| [index.js](./index.js) | JavaScript | Public JS wrapper that locates the WASM module, normalizes dictionary options, and exposes `init`, `query`, `version` and lifecycle methods. |
| [library.js](./library.js) | JavaScript | Emscripten JS library snippet adding `allocate`/`ALLOC_NORMAL` helpers for passing dictionary buffers into the module. |
| [migemo_wasm.js](./migemo_wasm.js) | Generated | Emscripten-generated ES module loader (web+node) that instantiates the WASM binary. |
| [migemo_wasm.wasm](./migemo_wasm.wasm) | WebAssembly | Emscripten-generated WebAssembly binary containing the migemo query engine. |
| [dict/](./dict/) | Directory | Runtime dictionary files: main `migemo-dict` and `migemo-dict-zh` plus the `roma2hira`, `hira2kata`, `han2zen`, `zen2han` sub-dictionaries, served over HTTP at runtime. |
| [test/](./test/) | Directory | Test harness (`index.html` + `test.js`) for the WASM module. |

# References
- [Parent Directory](../index.md)
