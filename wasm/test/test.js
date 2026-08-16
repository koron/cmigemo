import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import { fileURLToPath } from 'node:url';
import { init, query, close, isEnable, load, version, getModule, getInstance } from '../index.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, '../../');

async function runTests() {
  console.log('--- Starting WASM Migemo Tests ---');

  // Load sample dictionary files
  const dictDataPath = path.join(rootDir, 'test/test1/migemo-dict');
  const roma2hiraPath = path.join(rootDir, 'dict/roma2hira.dat');
  const hira2kataPath = path.join(rootDir, 'dict/hira2kata.dat');
  const han2zenPath = path.join(rootDir, 'dict/han2zen.dat');
  const zen2hanPath = path.join(rootDir, 'dict/zen2han.dat');

  const dictBuffer = fs.readFileSync(dictDataPath);
  const roma2hiraBuf = fs.readFileSync(roma2hiraPath);
  const hira2kataBuf = fs.readFileSync(hira2kataPath);
  const han2zenBuf = fs.readFileSync(han2zenPath);
  const zen2hanBuf = fs.readFileSync(zen2hanPath);

  // Test 1: Initialize with dictData Buffer and subdicts Buffers
  console.log('Test 1: Initialize with dictData Buffer & subdicts');
  const instance1 = await init({
    dictData: dictBuffer,
    dictPath: '/test-dict/migemo-dict',
    subdicts: {
      roma2hira: roma2hiraBuf,
      hira2kata: hira2kataBuf,
      han2zen: han2zenBuf,
      zen2han: zen2hanBuf
    }
  });

  assert.ok(instance1 > 0, 'Instance pointer should be a positive number');
  assert.strictEqual(getInstance(), instance1, 'getInstance() should match active handle');
  assert.strictEqual(isEnable(), true, 'isEnable() should be true');

  // Test 2: Query API returning RegExp
  console.log('Test 2: Query API returning RegExp');
  const rx1 = query('ak');
  assert.ok(rx1 instanceof RegExp, 'query() must return a native RegExp instance');
  console.log('Query "ak" -> RegExp:', rx1.toString());

  // Verify regex behavior on Japanese and Romaji strings
  assert.ok(rx1.test('赤'), 'RegExp should match "赤"');
  assert.ok(rx1.test('ak'), 'RegExp should match "ak"');

  const rx2 = query('n');
  assert.ok(rx2 instanceof RegExp, 'query("n") must return a native RegExp instance');
  console.log('Query "n" -> RegExp:', rx2.toString());
  assert.ok(rx2.test('なにぬねの'), 'RegExp should match "なにぬねの"');

  // Test 3: Check exported runtime methods on module
  console.log('Test 3: Verify Exported Runtime Utilities and C APIs');
  const mod = getModule();
  assert.ok(mod, 'Module instance should be defined');
  assert.ok(typeof mod.FS === 'object', 'FS should be exported on module');
  assert.ok(typeof mod.ccall === 'function', 'ccall should be exported on module');
  assert.ok(typeof mod.cwrap === 'function', 'cwrap should be exported on module');
  assert.ok(typeof mod.UTF8ToString === 'function', 'UTF8ToString should be exported on module');
  assert.ok(typeof mod.allocate === 'function', 'allocate should be exported on module');
  assert.ok(mod.ALLOC_NORMAL !== undefined, 'ALLOC_NORMAL should be exported on module');

  assert.ok(typeof mod._migemo_open === 'function', '_migemo_open C API should be exported');
  assert.ok(typeof mod._migemo_close === 'function', '_migemo_close C API should be exported');
  assert.ok(typeof mod._migemo_query === 'function', '_migemo_query C API should be exported');
  assert.ok(typeof mod._migemo_release === 'function', '_migemo_release C API should be exported');
  assert.ok(typeof mod._migemo_load === 'function', '_migemo_load C API should be exported');
  assert.ok(typeof mod._migemo_version === 'function', '_migemo_version C API should be exported');

  const ver = version();
  assert.ok(typeof ver === 'string' && ver.length > 0, 'version() should return a non-empty string');
  console.log('Version:', ver);

  // Test 4: Re-initialization (Singleton Lifecycle & dictPath input)
  console.log('Test 4: Re-initialization & Virtual FS Path');
  // Write files to virtual FS directly
  const virtualPath = '/virtual/opfs/migemo-dict';
  mod.FS.mkdirTree('/virtual/opfs');
  mod.FS.writeFile(virtualPath, dictBuffer);
  mod.FS.writeFile('/virtual/opfs/roma2hira.dat', roma2hiraBuf);

  const instance2 = await init({ dictPath: virtualPath });
  assert.ok(instance2 > 0, 'New instance pointer should be valid');
  assert.strictEqual(getInstance(), instance2, 'getInstance() should equal instance2');

  const rx3 = query('ak');
  assert.ok(rx3.test('明'), 'Query using re-initialized dictPath should match "明"');

  // Test 5: Close API
  console.log('Test 5: Close API');
  close();
  assert.strictEqual(getInstance(), 0, 'getInstance() should be 0 after close()');
  assert.strictEqual(isEnable(), false, 'isEnable() should be false after close()');
  assert.throws(() => query('ak'), /Migemo is not initialized/, 'query() should throw if not initialized');

  console.log('All WASM Migemo tests passed successfully!');
}

runTests().catch(err => {
  console.error('Test failed:', err);
  process.exit(1);
});
