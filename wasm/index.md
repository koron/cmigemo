---
type: Directory Index
title: "wasm Index"
description: "WebAssembly port of cMigemo with its Emscripten build, JavaScript wrapper API, and browser demo."
tags: [index]
---

# Overview
This directory contains the WebAssembly port of cMigemo: a CMake build that compiles the core C sources into an Emscripten module, the JavaScript wrapper that loads and drives that module, and a browser demo for verifying the port.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [.gitignore](./.gitignore) | Config | Excludes the generated `migemo_wasm.js` and `migemo_wasm.wasm` build artifacts from version control. |
| [CMakeLists.txt](./CMakeLists.txt) | Build Config | Builds the migemo C sources into the `migemo_wasm` Emscripten target and copies the generated `.js`/`.wasm` outputs into this directory. |
| [index.js](./index.js) | JavaScript API | Locates and instantiates the `createMigemoModule` factory and exposes a promise-based wrapper (`init`, `query`, `isEnable`, `close`) over the WASM migemo functions. |
| [library.js](./library.js) | Emscripten Library | Adds a `$allocate`/`$ALLOC_NORMAL` helper to the Emscripten runtime for copying byte buffers into WASM memory. |
| [migemo_wasm.js](./migemo_wasm.js) | Generated Glue | Emscripten-generated ES module exporting the `createMigemoModule` factory that loads the compiled binary. |
| [migemo_wasm.wasm](./migemo_wasm.wasm) | WebAssembly Binary | Compiled WASM binary of the migemo C library exporting `migemo_open`, `migemo_query`, `migemo_close`, and related functions. |
| [test/](./test/index.md) | Directory | Browser demo (`index.html`) and `test.js` that exercise dictionary initialization and incremental romaji queries against the WASM module. |

# References
- [Parent Directory](../index.md)
