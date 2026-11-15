// Run: node test/test_neopixel_web_effects.cjs
// Effect list, effect text and parameter labels of the web pages against web/assets/effects.json.
// The functions under test are copied from web/assets/studio.js and web/assets/neopixel.js by anchor and run in a vm sandbox.
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const root = path.resolve(__dirname, '..');
const catalog = JSON.parse(fs.readFileSync(path.join(root, 'web/assets/effects.json'), 'utf8'));
const studioSource = fs.readFileSync(path.join(root, 'web/assets/studio.js'), 'utf8').split(/\r?\n/);
const neoSource = fs.readFileSync(path.join(root, 'web/assets/neopixel.js'), 'utf8').split(/\r?\n/);

// The one line that contains the anchor
function line(lines, anchor) {
    const found = lines.filter(text => text.includes(anchor));
    assert.equal(found.length, 1, 'anchor not unique or missing: ' + anchor);
    return found[0];
}
// A function from its "function name(" line to the closing brace at the same indent
function fn(lines, name) {
    const start = lines.findIndex(text => new RegExp('^\\s*function ' + name + '\\(').test(text));
    assert(start >= 0, 'function missing: ' + name);
    const indent = lines[start].match(/^\s*/)[0];
    if (/\}\s*(\/\/.*)?$/.test(lines[start]) && !/\{\s*(\/\/.*)?$/.test(lines[start])) return lines[start];
    const end = lines.findIndex((text, i) => i > start && text === indent + '}');
    assert(end > start, 'function end missing: ' + name);
    return lines.slice(start, end + 1).join('\n');
}

// Independent expectation: ETS order = group, then number; 2D-only = dimensions 2
const etsOrder = list => list.map(e => e.id).sort((a, b) => {
    const ga = catalog.find(e => e.id === a).group || 0, gb = catalog.find(e => e.id === b).group || 0;
    return ga - gb || a - b;
});
const twoD = new Set(catalog.filter(e => e.dimensions === 2).map(e => e.id));
assert(twoD.size > 0 && twoD.has(19) && twoD.has(32), 'catalog without 2D-only effects');
const full = '1'.repeat(catalog.length);
const no2D = catalog.map(e => (twoD.has(e.id) ? '0' : '1')).join('');

// ---------------------------------------------------------------- studio.js
function element(tag) {
    return { tag, value: '', textContent: '', hidden: false, className: '' };
}
function studioSandbox() {
    const select = {
        children: [], value: undefined, _note: undefined, _key: undefined,
        parentElement: { after(node) { this.next = node; } },
        append(child) { this.children.push(child); },
        replaceChildren() { this.children = []; }
    };
    const context = { document: { createElement: element }, select };
    vm.createContext(context);
    vm.runInContext([
        "'use strict';",
        'let effects = [];',
        "const el = id => { if (id !== 'effect') throw new Error('unexpected element ' + id); return select; };",
        line(studioSource, 'const fxText = '),
        line(studioSource, 'const paramLabel = '),
        fn(studioSource, 'option'),
        fn(studioSource, 'effectOptions'),
        'function load(catalog, available) {',
        line(studioSource, "effects = catalog.filter(effect => available[effect.id] === '1')"),
        '}',
        'this.api = { load, fxText, paramLabel, effectOptions, effects: () => effects };'
    ].join('\n'), context);
    return context;
}

{
    const box = studioSandbox(), api = box.api;
    // Load: compiled-in effects only, ETS order
    api.load(catalog, no2D);
    assert.deepEqual(api.effects().map(e => e.id), etsOrder(catalog.filter(e => !twoD.has(e.id))));
    api.load(catalog, full);
    assert.deepEqual(api.effects().map(e => e.id), etsOrder(catalog));
    assert.notDeepEqual(etsOrder(catalog), catalog.map(e => e.id), 'catalog already in ETS order: the sort check proves nothing');

    // fxText: ETS entry by number, undefined for an unknown or compiled-out number
    for (const e of catalog) assert.equal(api.fxText(e.id), e.text || e.name);
    assert.equal(api.fxText(250), undefined);
    // paramLabel: ETS label when effect number, parameter number and firmware name match, else the firmware name
    for (const e of catalog)
        for (const p of e.parameters) assert.equal(api.paramLabel(e.id, { id: p.id, name: p.name }), p.label || p.name);
    const fire = catalog.find(e => e.parameters.length > 1 && e.parameters[0].label && e.parameters[0].label !== e.parameters[0].name);
    const p0 = fire.parameters[0], p1 = fire.parameters[1];
    assert.equal(api.paramLabel(fire.id, { id: p0.id, name: 'Renamed' }), 'Renamed');
    assert.equal(api.paramLabel(fire.id, { id: p1.id, name: p0.name }), p0.name);
    const other = catalog.find(e => e.id !== fire.id && !e.parameters.some(p => p.id === p0.id && p.name === p0.name));
    assert.equal(api.paramLabel(other.id, { id: p0.id, name: p0.name }), p0.name);
    assert.equal(api.paramLabel(250, { id: p0.id, name: p0.name }), p0.name);

    // effectOptions: 1D segment without 2D-only effects, the running effect always listed
    const options = () => box.select.children.map(o => o.value);
    api.effectOptions({ is2D: false, effect: 3 });
    assert.deepEqual(options(), etsOrder(catalog.filter(e => !twoD.has(e.id))));
    assert.equal(box.select.value, 3);
    assert.equal(box.select.parentElement.next.hidden, false);
    assert.deepEqual(box.select.children.map(o => o.textContent), options().map(id => api.fxText(id)));
    api.effectOptions({ is2D: false, effect: 22 });
    assert.deepEqual(options(), etsOrder(catalog.filter(e => !twoD.has(e.id) || e.id === 22)));
    assert.equal(box.select.value, 22);
    api.effectOptions({ is2D: true, effect: 22 });
    assert.deepEqual(options(), etsOrder(catalog));
    assert.equal(box.select.parentElement.next.hidden, true);
    // a list without 2D-only effects hides the note on a 1D segment too
    api.load(catalog, no2D);
    api.effectOptions({ is2D: false, effect: 0 });
    assert.equal(box.select.parentElement.next.hidden, true);
    console.log('PASS: studio.js effect list, effect text and parameter labels');
}

// ---------------------------------------------------------------- neopixel.js
{
    const fxList = line(neoSource, 'fxList = FX.filter(').match(/fxList = (FX\.filter\(function \(effect\) \{[^}]*\}\))/);
    assert(fxList, 'fxList expression not found');
    const context = {};
    vm.createContext(context);
    vm.runInContext([
        "'use strict';",
        'var FX = [];',
        line(neoSource, 'function esc(t)'),
        line(neoSource, 'function is1D(g)'),
        line(neoSource, 'function is2D(g)'),
        line(neoSource, 'function fxName(id)'),
        'function load(results) {',
        line(neoSource, "FX = results[0].filter(function (effect) { return results[1][effect.id] === '1'; })"),
        '}',
        // the effect row of one segment: has() = configured, ctlRow() = cell body only
        'function effectRow(s) {',
        '  var i = 0, rows = "", fxList = ' + fxList[1] + ';',
        '  var has = function () { return true; }, ctlRow = function (label, body) { return body; };',
        line(neoSource, "if (has(s, 'fx')) rows += ctlRow('Effekt'"),
        '  return rows;',
        '}',
        'this.api = { load: load, fxName: fxName, is2D: is2D, effectRow: effectRow, fx: function () { return FX; } };'
    ].join('\n'), context);
    const api = context.api;
    api.load([catalog, no2D]);
    assert.deepEqual(api.fx().map(e => e.id), etsOrder(catalog.filter(e => !twoD.has(e.id))));
    api.load([catalog, full]);
    assert.deepEqual(api.fx().map(e => e.id), etsOrder(catalog));
    for (const e of catalog) assert.equal(api.fxName(e.id), e.text || e.name);
    assert.equal(api.fxName(250), 'Effekt 250');

    // geometry [width, height, depth, topology] as LedGeometry::is2D
    assert.equal(api.is2D([0]), false);
    assert.equal(api.is2D([8, 8, 0, 1]), true);
    assert.equal(api.is2D([8, 8, 4, 5]), false);
    assert.equal(api.is2D([8, 8, 4, 6]), true);
    assert.equal(api.is2D([8, 1, 0, 1]), false);

    const parse = html => {
        const list = [...html.matchAll(/<option value="(\d+)"( selected)?>([^<]*)<\/option>/g)];
        return { ids: list.map(m => +m[1]), selected: list.filter(m => m[2]).map(m => +m[1]), texts: list.map(m => m[3]), note: html.includes('2D-Effekte nur bei 2D-Segmenten') };
    };
    // 1D segment: no 2D-only effect, selection by id even when the shown name belongs to another effect
    let row = parse(api.effectRow({ geo: [0], fxId: 3, fx: api.fxName(0) }));
    assert.deepEqual(row.ids, etsOrder(catalog.filter(e => !twoD.has(e.id))));
    assert.deepEqual(row.selected, [3]);
    assert.equal(row.note, true);
    // 1D segment running a 2D-only effect: that effect stays listed and selected
    row = parse(api.effectRow({ geo: [0], fxId: 22, fx: api.fxName(22) }));
    assert.deepEqual(row.ids, etsOrder(catalog.filter(e => !twoD.has(e.id) || e.id === 22)));
    assert.deepEqual(row.selected, [22]);
    // 2D segment: every effect, no note
    row = parse(api.effectRow({ geo: [8, 8, 0, 1], fxId: 22, fx: api.fxName(22) }));
    assert.deepEqual(row.ids, etsOrder(catalog));
    assert.deepEqual(row.selected, [22]);
    assert.equal(row.note, false);
    const html = t => String(t).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;').replace(/'/g, '&#39;');
    assert.deepEqual(row.texts, row.ids.map(id => html((catalog.find(e => e.id === id).text) || catalog.find(e => e.id === id).name)));
    console.log('PASS: neopixel.js effect list, selection by id and effect text');
}
