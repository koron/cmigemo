---
type: Directory Index
title: "wasm Index"
description: "WebAssembly build of C/Migemo with its JavaScript API wrapper, demo page, dictionaries, and test files."
tags: [index]
---

# Overview
This directory contains the Emscripten/WebAssembly port of C/Migemo: a CMake build definition, the compiled `.js`/`.wasm` artifacts, a high-level JavaScript API, a browser demo page, the dictionary files, and a small test harness.

# Directory Contents

| File / Path | Type | Description |
| :--- | :--- | :--- |
| [CMakeLists.txt](./CMakeLists.txt) | CMake | Builds the `migemo_wasm` Emscripten executable that exports the migemo API functions and copies the generated `.js`/`.wasm` artifacts back into this directory. |
| [demo.html](./demo.html) | HTML | Demo page that loads the WASM module from local dictionary files over HTTP and shows interactive browser-console usage examples. |
| [dict/](./dict/) | Directory | Holds the main dictionaries (`migemo-dict`, `migemo-dict-zh`) and the `roma2hira.dat`, `hira2kata.dat`, `han2zen.dat`, `zen2han.dat` sub-dictionaries fetched by the demo. |
| [index.js](./index.js) | JavaScript | High-level API that loads the Emscripten module, writes dictionary data into the virtual filesystem, and exposes `init`, `query`, `close`, `version`, and related functions. |
| [library.js](./library.js) | JavaScript | Emscripten `--js-library` file that adds `$ALLOC_NORMAL` and `$allocate` helpers for copying memory into the WASM heap. |
| [migemo_wasm.js](./migemo_wasm.js) | Build Artifact | Emscripten-generated JS loader that exposes the `createMigemoModule` factory for the compiled module. |
| [migemo_wasm.wasm](./migemo_wasm.wasm) | Build Artifact | Compiled WebAssembly binary of the C/Migemo library consumed by `migemo_wasm.js`. |
| [test/](./test/) | Directory | Contains `index.html` and `test.js`, a lightweight browser-based test harness for the WASM module. |

# References
- [Parent Directory](../index.md)
