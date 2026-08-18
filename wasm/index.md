---
type: Directory Index
title: "wasm Index"
description: "WebAssembly build of the cmigemo library with emscripten wrappers and a browser test page."
tags: [index]
---

# Overview

This directory contains a standalone CMake build that compiles the cmigemo library to WebAssembly, along with the emscripten-generated JS wrapper, browser-side JS entry points, and a small HTML test harness.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | Build | Standalone CMake rules that build the migemo_wasm executable with Emscripten. |
| [index.js](./index.js) | Source | Browser/Node entry point that loads and instantiates the migemo WebAssembly module. |
| [library.js](./library.js) | Source | Emscripten library function definitions linked into the WASM module. |
| [migemo_wasm.js](./migemo_wasm.js) | Build Output | Emscripten-generated JavaScript loader for the compiled WASM module. |
| [migemo_wasm.wasm](./migemo_wasm.wasm) | Build Output | Compiled WebAssembly binary of the migemo library. |
| [.gitignore](./.gitignore) | Configuration | Git exclusion rules keeping the generated WASM build artifacts out of version control. |
| [test/](./test/) | Directory Index | Browser test harness (HTML page and JS) for the WASM build. |

# References
- [Parent Directory](../index.md)
