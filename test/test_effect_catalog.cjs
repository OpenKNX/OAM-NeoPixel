const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const cp = require('node:child_process');

const root = path.resolve(__dirname, '..');
const catalog = JSON.parse(fs.readFileSync(path.join(root, 'web/assets/effects.json'), 'utf8'));
const effectsDir = path.join(root, 'lib/OFM-NeoPixel/src/effects');
const methods = ['Name', 'ParameterCount', 'ParameterName', 'ParameterType', 'ParameterMin', 'ParameterMax', 'ParameterDefault', 'ParameterDefaultText', 'EnumValueCount', 'EnumValueName'];
function body(source, name) {
    const match = new RegExp('\\bget' + name + '\\s*\\([^)]*\\)[^{;]*\\{').exec(source);
    if (!match) return null;
    const start = match.index;
    let depth = 1, cursor = start + match[0].length;
    const tokens = /"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\/\/[^\n]*|\/\*[\s\S]*?\*\/|[{}]/g;
    tokens.lastIndex = cursor;
    for (let token; (token = tokens.exec(source));) {
        if (token[0] === '{') depth++;
        if (token[0] === '}' && --depth === 0) {
            return source.slice(start, tokens.lastIndex).replace(/\boverride\b/g, '').replace(/\)\s*const\s*\{/g, ') {');
        }
    }
    throw Error('Unterminated method ' + name);
}
const defaults = {
    Name: 'const char* getName(const char* lang=nullptr) { return ""; }',
    ParameterCount: 'uint8_t getParameterCount() { return 0; }',
    ParameterName: 'const char* getParameterName(uint8_t index) { return ""; }',
    ParameterType: 'ParameterType getParameterType(uint8_t index) { return ParameterType::PARAM_UINT8; }',
    ParameterMin: 'uint32_t getParameterMin(uint8_t index) { return 0; }',
    ParameterMax: 'uint32_t getParameterMax(uint8_t index) { return 255; }',
    ParameterDefault: 'uint32_t getParameterDefault(uint8_t index) { return 0; }',
    ParameterDefaultText: 'const char* getParameterDefaultText(uint8_t index) { return nullptr; }',
    EnumValueCount: 'uint8_t getEnumValueCount(uint8_t index) { return 0; }',
    EnumValueName: 'const char* getEnumValueName(uint8_t index,uint8_t value) { return nullptr; }'
};
const pool = fs.readFileSync(path.join(effectsDir, 'EffectPool.cpp'), 'utf8');
const headers = fs.readdirSync(effectsDir).filter(name => /Effect.*\.h$|.*Effect\.h$/.test(name) && name !== 'Effect.h' && name !== 'EffectPool.h');
const sources = headers.map(name => fs.readFileSync(path.join(effectsDir, name), 'utf8'));
const mapping = fs.readFileSync(path.join(root, 'src/EffectTypeMapping.h'), 'utf8');
const cases = [...mapping.matchAll(/case PT_NEOEffectType::(\w+): return EffectPool::get(\w+)\(\)/g)];
let cpp = '#include <cstdint>\n#include <cstring>\n#include <iostream>\n#include "ParameterType.h"\n#define EFFECT_NAME_DE_EN(de,en) (lang && strcmp(lang,"de")==0 ? de : en)\n';
const fontCount = fs.readFileSync(path.join(effectsDir, 'ScrollTextFonts.h'), 'utf8').match(/kScrollFontCount\s*=\s*(\d+)/)[1];
cpp += 'constexpr uint8_t kScrollFontCount=' + fontCount + ';\n';
for (let id = 0; id < cases.length; id++) {
    const method = cases[id][2];
    const source = sources.find(text => new RegExp('class\\s+(?:Effect' + method + '|' + method + 'Effect)\\s*:').test(text));
    assert(source, 'Missing source for ' + method);
    cpp += 'namespace effect' + id + ' {\n';
    cpp += [...source.matchAll(/enum\s*(?:class\s+)?\w*\s*(?::\s*\w+)?\s*\{[^}]*\}\s*;/g)].map(match => match[0]).join('\n') + '\n';
    cpp += [...source.matchAll(/(?:static\s+)?(?:constexpr|const)\s+[^;{}=]+\s*=\s*"(?:\\.|[^"\\])*"\s*;/g)].map(match => match[0]).join('\n') + '\n';
    cpp += methods.map(name => {
        const methodBody = body(source, name);
        return methodBody ? defaults[name].split('get' + name)[0] + methodBody : defaults[name];
    }).join('\n') + '\n}\n';
}
cpp += 'int main() {\n';
for (let id = 0; id < cases.length; id++) {
    cpp += '{ using namespace effect' + id + '; std::cout << "E\\t' + id + '\\t" << getName("en") << "\\t" << unsigned(getParameterCount()) << "\\n";\n';
    cpp += 'for(uint8_t param=0;param<getParameterCount();++param) { auto type=getParameterType(param); auto text=getParameterDefaultText(param); std::cout << "P\\t" << unsigned(param) << "\\t" << getParameterName(param) << "\\t" << (type==ParameterType::PARAM_STRING ? "text" : type==ParameterType::PARAM_BOOL ? "bool" : type==ParameterType::PARAM_ENUM ? "enum" : "number") << "\\t" << getParameterMin(param) << "\\t" << getParameterMax(param) << "\\t"; if(type==ParameterType::PARAM_STRING) std::cout << (text ? text : ""); else std::cout << getParameterDefault(param); for(uint8_t option=0;type==ParameterType::PARAM_ENUM && option<getEnumValueCount(param);++option) { auto name=getEnumValueName(param,option); std::cout << "\\t" << (name ? name : ""); } std::cout << "\\n"; } }\n';
}
cpp += '}\n';
const temp = fs.mkdtempSync(path.join(os.tmpdir(), 'neopixel-catalog-'));
try {
    const source = path.join(temp, 'metadata.cpp'), exe = path.join(temp, 'metadata.exe');
    fs.writeFileSync(source, cpp);
    cp.execFileSync('clang++', ['-std=c++17', '-I', effectsDir, source, '-o', exe], { stdio: 'inherit' });
    const actual = [];
    let current;
    for (const line of cp.execFileSync(exe, { encoding: 'utf8' }).trimEnd().split(/\r?\n/)) {
        const fields = line.split('\t');
        if (fields[0] === 'E') { current = { id: +fields[1], name: fields[2], parameters: [] }; actual.push(current); }
        else current.parameters.push({ id: +fields[1], name: fields[2], type: fields[3], min: +fields[4], max: +fields[5], value: fields[3] === 'text' ? fields[6] : +fields[6], options: fields.slice(7) });
    }
    assert.deepEqual(catalog.map(({ id, name, parameters }) => ({ id, name, parameters })), actual);
    console.log('PASS: generated catalog matches original C++ metadata for ' + actual.length + ' effects.');
    const headerPath = path.join(root, 'src/WebUi/NeoPixelEffectCatalog.generated.h');
    const header = fs.readFileSync(headerPath, 'utf8');
    const compressed = Buffer.from([...header.split('    };')[0].matchAll(/0x([0-9a-f]{2})/g)].map(match => parseInt(match[1], 16)));
    assert.deepEqual(JSON.parse(require('node:zlib').gunzipSync(compressed)), catalog);
    fs.writeFileSync(source, '#include <iostream>\n#include "NeoPixelEffectCatalog.generated.h"\nint main() { std::cout << NeoPixelEffectCatalog::availableJson; for(unsigned id=0;id<255;++id) { bool expected=id<sizeof(NeoPixelEffectCatalog::available)-1 && NeoPixelEffectCatalog::available[id]==\'1\'; if(NeoPixelEffectCatalog::isAvailable(id)!=expected) return 1; } }\n');
    for (const [defines, expected] of [
        [[], '1'.repeat(33)],
        [['NEOPIXEL_DISABLE_2D'], '1'.repeat(20) + '0'.repeat(13)],
        [['NEOPIXEL_MINIMAL_EFFECTS'], '1'.repeat(8) + '0'.repeat(11) + '1'.repeat(14)],
        [['NEOPIXEL_DISABLE_WIPE', 'NEOPIXEL_DISABLE_2D'], '10' + '1'.repeat(18) + '0'.repeat(13)]
    ]) {
        cp.execFileSync('clang++', ['-std=c++17', '-I', path.dirname(headerPath), ...defines.map(name => '-D' + name), source, '-o', exe], { stdio: 'inherit' });
        assert.equal(JSON.parse(cp.execFileSync(exe, { encoding: 'utf8' })), expected);
    }
    console.log('PASS: gzip round-trip and compiled availability masks (full, no-2D, minimal, individual flag).');
} finally { fs.rmSync(temp, { recursive: true, force: true }); }