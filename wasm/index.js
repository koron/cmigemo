import createMigemoModule from './migemo_wasm.js';

let moduleInstance = null;
let migemoInstance = 0;

/**
 * Gets the Emscripten module factory function.
 * @returns {Function}
 */
function getModuleFactory() {
  if (typeof createMigemoModule === 'function') {
    return createMigemoModule;
  }
  if (typeof globalThis !== 'undefined' && typeof globalThis.createMigemoModule === 'function') {
    return globalThis.createMigemoModule;
  }
  throw new Error("Unable to locate createMigemoModule factory function.");
}

/**
 * Normalizes user options into a standard object structure.
 * @param {Object|string|Uint8Array|ArrayBuffer} options
 * @returns {Object}
 */
function normalizeOptions(options) {
  if (typeof options === 'string') {
    return { dictPath: options };
  }
  if (options instanceof Uint8Array || options instanceof ArrayBuffer) {
    return { dictData: options };
  }
  if (typeof options === 'object' && options !== null) {
    return { ...options };
  }
  return {};
}

/**
 * Ensures directory structure exists in Emscripten Virtual File System.
 * @param {Object} FS
 * @param {string} filePath
 */
function ensureDirectory(FS, filePath) {
  const parts = filePath.split('/').filter(Boolean);
  let currentPath = '';
  for (let i = 0; i < parts.length - 1; i++) {
    currentPath += '/' + parts[i];
    try {
      FS.mkdir(currentPath);
    } catch (e) {
      // Ignore EEXIST
    }
  }
}

/**
 * Initializes C/Migemo WebAssembly module and loads dictionary.
 * @param {Object|string|Uint8Array|ArrayBuffer} options
 * @returns {Promise<number>} Returns the C pointer singleton for the Migemo instance.
 */
export async function init(options = {}) {
  const opts = normalizeOptions(options);

  // Close previous C instance if active to prevent memory leaks
  if (migemoInstance) {
    close();
  }

  // Load Emscripten module if not yet loaded
  if (!moduleInstance) {
    let factory = opts.createMigemoModule;
    if (!factory) {
      factory = getModuleFactory();
    }
    const defaultModuleOpts = {
      locateFile: (path, scriptDirectory) => {
        if (typeof import.meta !== 'undefined' && import.meta.url) {
          return new URL(path, import.meta.url).href;
        }
        return (scriptDirectory || '') + path;
      }
    };
    moduleInstance = await factory({ ...defaultModuleOpts, ...opts.moduleOpts });
  }

  const FS = moduleInstance.FS;
  let dictPath = opts.dictPath || null;

  // Handle options.dictData
  if (opts.dictData) {
    const data = opts.dictData instanceof Uint8Array ? opts.dictData : new Uint8Array(opts.dictData);
    dictPath = opts.dictPath || '/migemo-dict';

    ensureDirectory(FS, dictPath);

    try {
      FS.unlink(dictPath);
    } catch (e) {
      // Ignore ENOENT
    }

    if (typeof FS.createDataFile === 'function') {
      const lastSlash = dictPath.lastIndexOf('/');
      const parentDir = lastSlash > 0 ? dictPath.substring(0, lastSlash) : '/';
      const fileName = lastSlash >= 0 ? dictPath.substring(lastSlash + 1) : dictPath;
      try {
        FS.createDataFile(parentDir, fileName, data, true, true, true);
      } catch (e) {
        // Fallback to FS.writeFile
        FS.writeFile(dictPath, data);
      }
    } else {
      FS.writeFile(dictPath, data);
    }
  }

  // Handle options.subdicts
  if (opts.subdicts && typeof opts.subdicts === 'object') {
    const baseDir = dictPath ? (dictPath.substring(0, dictPath.lastIndexOf('/')) || '/') : '/';
    const standardNames = {
      roma2hira: 'roma2hira.dat',
      hira2kata: 'hira2kata.dat',
      han2zen: 'han2zen.dat',
      zen2han: 'zen2han.dat'
    };

    for (const [key, val] of Object.entries(opts.subdicts)) {
      if (val instanceof Uint8Array || val instanceof ArrayBuffer) {
        const fileName = standardNames[key] || (key.endsWith('.dat') ? key : `${key}.dat`);
        const subPath = baseDir === '/' ? `/${fileName}` : `${baseDir}/${fileName}`;
        const subData = val instanceof Uint8Array ? val : new Uint8Array(val);

        ensureDirectory(FS, subPath);
        try { FS.unlink(subPath); } catch (e) {}
        FS.writeFile(subPath, subData);
      }
    }
  }

  const migemoOpen = moduleInstance.cwrap('migemo_open', 'number', ['string']);
  migemoInstance = migemoOpen(dictPath);

  if (!migemoInstance) {
    throw new Error(`Failed to initialize Migemo with dictPath: ${dictPath}`);
  }

  return migemoInstance;
}

/**
 * Converts input query string into a JavaScript RegExp object.
 * @param {string} pattern
 * @returns {RegExp}
 */
export function query(pattern) {
  if (!migemoInstance || !moduleInstance) {
    throw new Error("Migemo is not initialized. Call init() first.");
  }

  const migemoQuery = moduleInstance.cwrap('migemo_query', 'number', ['number', 'string']);
  const migemoRelease = moduleInstance.cwrap('migemo_release', 'void', ['number', 'number']);

  const resPtr = migemoQuery(migemoInstance, pattern);
  if (!resPtr) {
    return new RegExp(pattern);
  }

  const resStr = moduleInstance.UTF8ToString(resPtr);
  migemoRelease(migemoInstance, resPtr);

  return new RegExp(resStr);
}

/**
 * Closes and frees the active Migemo instance pointer.
 */
export function close() {
  if (migemoInstance && moduleInstance) {
    const migemoClose = moduleInstance.cwrap('migemo_close', 'void', ['number']);
    migemoClose(migemoInstance);
    migemoInstance = 0;
  }
}

/**
 * Checks whether dictionary is loaded and enabled.
 * @returns {boolean}
 */
export function isEnable() {
  if (!migemoInstance || !moduleInstance) return false;
  const migemoIsEnable = moduleInstance.cwrap('migemo_is_enable', 'number', ['number']);
  return Boolean(migemoIsEnable(migemoInstance));
}

/**
 * Loads an additional dictionary file into the active instance.
 * @param {number} dictId
 * @param {string} dictPath
 * @returns {number}
 */
export function load(dictId, dictPath) {
  if (!migemoInstance || !moduleInstance) {
    throw new Error("Migemo is not initialized. Call init() first.");
  }
  const migemoLoad = moduleInstance.cwrap('migemo_load', 'number', ['number', 'number', 'string']);
  return migemoLoad(migemoInstance, dictId, dictPath);
}

/**
 * Returns the underlying Emscripten module instance.
 * @returns {Object|null}
 */
export function getModule() {
  return moduleInstance;
}

/**
 * Returns the active C Migemo pointer singleton.
 * @returns {number}
 */
export function getInstance() {
  return migemoInstance;
}

export default {
  init,
  query,
  close,
  isEnable,
  load,
  getModule,
  getInstance
};
