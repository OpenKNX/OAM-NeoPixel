// Run: node --expose-internals scripts/test_ets_js_syntax.cjs [script.js ...]
// Uses Acorn bundled with Node.js, or an installed acorn package.
const fs = require('node:fs');
const path = require('node:path');
let acorn;
try { acorn = require('acorn'); }
catch (_) { acorn = require('internal/deps/acorn/acorn/dist/acorn'); }
const files = process.argv.slice(2);
if (!files.length) files.push(path.join(__dirname, '../src/NeoPixel.script.js'), path.join(__dirname, 'test_neopixel_studio_ets_wsh.js'));
for (const file of files) {
    acorn.parse(fs.readFileSync(file, 'utf8'), {ecmaVersion:3, allowReserved:'never'});
    console.log('ES3 syntax passed: ' + file);
}
