#pragma once

static const char kNeoPixelStudioHtml[] = R"HTML(
<div class="container" id="neopixel-studio">
<h1>NeoPixel Studio</h1>
<p class="meta">Live colours and effects for the segments configured in ETS. Changes apply automatically. Disable automatic apply to edit a draft before applying it. Startup behaviour follows the ETS settings.</p>
<p id="studio-status" role="status" aria-live="polite">Connecting…</p>
<noscript>JavaScript is required to control NeoPixel Studio.</noscript>
<form id="studio-form" hidden>
<div class="studio-actions"><button type="button" id="studio-auto-apply" aria-pressed="true">Automatic apply: On</button><button type="button" id="studio-apply">Apply live</button><button type="button" id="studio-reset">Reset draft</button><button type="button" id="studio-save">Save to device</button><button type="button" id="studio-load">Load from device</button></div>
<h2>Segment</h2>
<label for="studio-segment">Select segment</label>
<select id="studio-segment"></select>
<p class="meta" id="studio-segment-state"></p>
<fieldset id="studio-controls">
<legend class="studio-sr">Live controls</legend>
<h2>Effect</h2>
<div class="studio-row"><label for="studio-effect">Effect</label><select id="studio-effect"></select></div>
<div class="studio-row"><label for="studio-power">Segment power</label><input type="checkbox" id="studio-power"></div>
<h2>Colours</h2>
<div class="studio-row"><label for="studio-brightness">Brightness</label><input id="studio-brightness" type="range" min="0" max="255"><output id="studio-brightness-value" for="studio-brightness"></output></div>
<div id="studio-colours"></div>

<h2>Effect parameters</h2>
<div id="studio-parameters"></div>
<h2>Scenes</h2><div id="studio-scenes"></div>
</fieldset>
<p id="studio-draft" class="meta"></p><details><summary>Review pending changes</summary><pre id="studio-payload"></pre></details>
<p class="meta">Automatic apply updates the live output after each edit. When disabled, use Apply live to preview the draft. Save applies it and persists visual settings and scenes for ETS import. Load discards its draft and refreshes device values. ETS remains the configuration source.</p>
</form>
</div>
<style>
#neopixel-studio #studio-status {font-size:.88em;margin-bottom:1em;min-height:1.2em;color:#666}
#neopixel-studio #studio-status[data-error="true"] {color:#b3261e}
#neopixel-studio fieldset {border:0;padding:0;margin:0;min-width:0}
#neopixel-studio .studio-row {display:flex;align-items:center;gap:12px;padding:10px 0;border-bottom:1px solid #eee;min-height:48px}
#neopixel-studio .studio-row label {width:200px;flex-shrink:0;color:#666;font-size:.88em}
#neopixel-studio input[type="range"] {flex:1;max-width:340px;min-width:60px;accent-color:#449841}
#neopixel-studio input[type="checkbox"] {accent-color:#449841}
#neopixel-studio input[type="color"] {width:64px;height:36px;padding:2px;cursor:pointer}
#neopixel-studio input[type="text"], #neopixel-studio select {max-width:100%;min-width:0}
#neopixel-studio input[type="text"] {width:340px}
#neopixel-studio output {font-size:.8em;color:#666;min-width:3.5em}
#neopixel-studio #studio-segment-state {margin-top:10px;margin-bottom:0}
#neopixel-studio .studio-sr {position:absolute;width:1px;height:1px;overflow:hidden;clip-path:inset(50%)}
@media(max-width:640px) {
#neopixel-studio .studio-row {flex-wrap:wrap;gap:8px}
#neopixel-studio .studio-row label {width:100%}
#neopixel-studio input[type="text"] {width:100%}
}

#neopixel-studio #studio-colours {display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:20px;margin-top:20px}
#neopixel-studio .studio-picker {border:1px solid #ddd;padding:16px;min-width:0}
#neopixel-studio .studio-picker-heading {display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:12px}
#neopixel-studio .studio-picker h3 {font-size:.9em;font-weight:600}
#neopixel-studio .studio-swatch {width:32px;height:24px;border:1px solid #bbb}
#neopixel-studio .studio-picker-tabs {display:flex;margin-bottom:16px}
#neopixel-studio .studio-picker-tabs button {flex:1;border:1px solid #bbb;background:#fff;color:#555;cursor:pointer}
#neopixel-studio .studio-picker-tabs button[aria-pressed="true"] {background:#449841;border-color:#449841;color:#fff}
#neopixel-studio .studio-picker-tabs button:disabled {opacity:.5;cursor:default}
#neopixel-studio .studio-spectrum {height:160px;position:relative;touch-action:none;cursor:crosshair;background-image:linear-gradient(to top,#000,transparent),linear-gradient(to right,#fff,transparent);outline-offset:3px}
#neopixel-studio .studio-spectrum[aria-disabled="true"] {opacity:.5;cursor:default}
#neopixel-studio .studio-picker-cursor {position:absolute;width:12px;height:12px;border:2px solid #fff;border-radius:50%;box-shadow:0 0 2px 1px #333;transform:translate(-50%,-50%);pointer-events:none}
#neopixel-studio .studio-picker-label {display:block;font-size:.8em;color:#666;margin:14px 0 6px}
#neopixel-studio .studio-picker input[type="range"] {display:block;width:100%;max-width:none;margin:0}
#neopixel-studio .studio-hue, #neopixel-studio .studio-white-tone {appearance:none;height:14px;padding:0;border:1px solid #bbb;border-radius:0;background:linear-gradient(to right,#f00,#ff0,#0f0,#0ff,#00f,#f0f,#f00)}
#neopixel-studio .studio-white-tone {background:var(--studio-white-gradient)}
#neopixel-studio .studio-hue::-webkit-slider-thumb, #neopixel-studio .studio-white-tone::-webkit-slider-thumb {appearance:none;width:12px;height:22px;background:#fff;border:1px solid #555;border-radius:2px}
#neopixel-studio .studio-hue::-moz-range-thumb, #neopixel-studio .studio-white-tone::-moz-range-thumb {width:12px;height:22px;background:#fff;border:1px solid #555;border-radius:2px}
#neopixel-studio .studio-hex-row {display:flex;align-items:center;gap:10px;margin-top:14px}
#neopixel-studio .studio-hex-row input[type="text"] {width:110px;min-width:0;font-family:monospace}
#neopixel-studio .studio-white-preview {height:160px;border:1px solid #ddd;display:flex;align-items:flex-end;padding:10px;font-size:.75em;color:#555}
#neopixel-studio .studio-picker-ends {display:flex;justify-content:space-between;font-size:.75em;color:#666;margin-top:6px}
#neopixel-studio .studio-white-help, #neopixel-studio .studio-mixed {margin:14px 0 0;line-height:1.4}
#neopixel-studio .studio-picker output {float:right}
#neopixel-studio .studio-actions {display:flex;gap:10px;flex-wrap:wrap;margin:20px 0 12px}
#neopixel-studio #studio-save {background:#449841;color:white;border:1px solid #449841;cursor:pointer}
#neopixel-studio #studio-save:disabled {opacity:.5;cursor:default}
@media(max-width:800px) { #neopixel-studio #studio-colours {grid-template-columns:1fr} }

#neopixel-studio .studio-numeric {width:80px;min-width:0}
#neopixel-studio .studio-rgb-numbers {display:flex;gap:10px;margin-top:12px}
#neopixel-studio .studio-rgb-numbers label {flex:1;min-width:0;font-size:.8em;color:#666}
#neopixel-studio .studio-rgb-numbers input {display:block;width:100%;min-width:0;margin-top:5px}
#neopixel-studio #studio-scenes .studio-scene {display:flex;flex-wrap:wrap;gap:8px;align-items:center;padding:10px 0;border-bottom:1px solid #eee;font-size:.88em}
#neopixel-studio .studio-scene strong {margin-right:auto}
#neopixel-studio #studio-payload {font-size:.75em;white-space:pre-wrap;overflow-wrap:anywhere;max-height:240px;overflow:auto;margin:12px 0}
</style>
<script defer src="/neopixel/studio.js"></script>
)HTML";

static const char kNeoPixelStudioJs[] = R"JS(
(() => {
    'use strict';
    const root = document.getElementById('neopixel-studio');
    if (!root) return;
    const el = id => document.getElementById('studio-' + id);
    let state, liveState, effects = [], selected = 0, busy = false, autoApply = true, signature = '', timer;
    const dirty = new Set(), sceneActions = new Map();
    const clone = value => JSON.parse(JSON.stringify(value));
    const status = (text, error = false) => {
        el('status').textContent = text;
        el('status').dataset.error = String(error);
    };
    async function request(path, options) {
        const controller = new AbortController();
        const timeout = setTimeout(() => controller.abort(), 6000);
        try {
            const response = await fetch('/neopixel/api/' + path, {
                cache: 'no-store', ...options, signal: controller.signal
            });
            const data = await response.json();
            if (!response.ok) throw new Error(data.error || 'Device request failed (' + response.status + ')');
            return data;
        } finally { clearTimeout(timeout); }
    }
    const current = () => state?.segments.find(segment => segment.id === selected);
    function disabled() {
        const segment = current();
        el('controls').disabled = busy || !segment || segment.locked || segment.detailsLoaded === false;
        el('segment').disabled = busy;
        el('save').disabled = busy || !segment;
        el('load').disabled = busy || !segment;
        el('apply').disabled = busy || !segment || segment.locked || !dirty.has(selected);
        el('reset').disabled = busy || !dirty.has(selected);
        for (const square of root.querySelectorAll('.studio-spectrum')) {
            square.tabIndex = el('controls').disabled ? -1 : 0;
            square.setAttribute('aria-disabled', String(el('controls').disabled));
        }
    }
    function option(select, value, name) {
        const item = document.createElement('option');
        item.value = value;
        item.textContent = name;
        select.append(item);
    }
    function row(parent, name, input) {
        const item = document.createElement('div');
        item.className = 'studio-row';
        const label = document.createElement('label');
        label.htmlFor = input.id;
        label.textContent = name;
        item.append(label, input);
        parent.append(item);
        return item;
    }
    function control(parent, name, id, type, value, send, min = 0, max = 255) {
        const input = document.createElement('input');
        input.id = 'studio-' + id;
        input.type = type;
        if (type === 'checkbox') input.checked = Boolean(value);
        else input.value = value;
        if (type === 'range' || type === 'number') {
            input.min = min;
            input.max = max;
            input.step = 1;
        }
        const item = row(parent, name, input);
        if (type === 'range') {
            const number = document.createElement('input');
            number.type = 'number'; number.required = true; number.min = min; number.max = max; number.step = 1;
            number.id = input.id + '-number'; number.value = value;
            number.setAttribute('aria-label', name + ' numeric value');
            number.className = 'studio-numeric';
            input.addEventListener('input', () => { number.value = input.value; });
            number.addEventListener('change', () => {
                if (!number.checkValidity() || !Number.isInteger(Number(number.value))) { number.reportValidity(); return; }
                input.value = number.value; send(Number(number.value));
            });
            item.append(number);
        }
        if (type === 'text') input.maxLength = 240;
        input.addEventListener('change', () => send(type === 'checkbox' ? Number(input.checked) : type === 'range' || type === 'number' ? Number(input.value) : input.value));
        return input;
    }
    const pickerMemory = new Map();
    const pickers = new Map();
    function rgbToHsv(hex) {
        const values = hex.match(/[0-9a-f]{2}/gi).map(v => parseInt(v, 16) / 255);
        const [r, g, b] = values, max = Math.max(...values), min = Math.min(...values), delta = max - min;
        let hue = 0;
        if (delta) {
            if (max === r) hue = ((g - b) / delta + 6) % 6;
            else if (max === g) hue = (b - r) / delta + 2;
            else hue = (r - g) / delta + 4;
        }
        return [hue * 60, max ? delta / max : 0, max];
    }
    function hsvToRgb(hue, saturation, value) {
        const chroma = value * saturation, x = chroma * (1 - Math.abs((hue / 60) % 2 - 1)), m = value - chroma;
        const rgb = hue < 60 ? [chroma, x, 0] : hue < 120 ? [x, chroma, 0] : hue < 180 ? [0, chroma, x] : hue < 240 ? [0, x, chroma] : hue < 300 ? [x, 0, chroma] : [chroma, 0, x];
        return '#' + rgb.map(v => Math.round((v + m) * 255).toString(16).padStart(2, '0')).join('');
    }
    function kelvinRgb(kelvin) {
        const t = Math.max(2000, Math.min(6500, kelvin)) / 100;
        const clamp = v => Math.round(Math.max(0, Math.min(255, v)));
        return [255, clamp(99.4708025861 * Math.log(t) - 161.1195681661), t <= 19 ? 0 : clamp(138.5177312231 * Math.log(t - 10) - 305.0447927307)];
    }
    function whiteTone(blend) { return 'rgb(' + kelvinRgb(2000 + 45 * blend).join(',') + ')'; }
    function colourPicker(key, name, colour) {
        const memoryKey = selected + ':' + key;
        if (!pickerMemory.has(memoryKey)) pickerMemory.set(memoryKey, {
            mode: colour.ww + colour.cw > 0 ? colour.rgb === '#000000' ? 'white' : 'mixed' : 'rgb',
            rgb: colour.rgb === '#000000' ? '#449841' : colour.rgb,
            level: Math.min(255, colour.ww + colour.cw) || 255,
            blend: colour.ww + colour.cw ? Math.round(100 * colour.cw / (colour.ww + colour.cw)) : 50
        });
        const memory = pickerMemory.get(memoryKey);
        const panel = document.createElement('section');
        panel.className = 'studio-picker';
        // Keys and names are fixed application strings. Device values are assigned through DOM properties.
        panel.innerHTML = `<div class="studio-picker-heading"><h3>${name}</h3><span class="studio-swatch" aria-hidden="true"></span></div>
<div class="studio-picker-tabs" role="group" aria-label="${name} mode">
<button type="button" id="studio-${key}-rgb-mode">RGB</button><button type="button" id="studio-${key}-white-mode">White</button><button type="button" id="studio-${key}-mixed-mode">Mix</button></div>
<div class="studio-rgb-picker">
<div id="studio-${key}-spectrum" class="studio-spectrum" tabindex="0" role="group" aria-label="${name} saturation and value. Left and right change saturation; up and down change value."><span class="studio-picker-cursor"></span></div>
<label class="studio-picker-label" for="studio-${key}-hue">Hue</label><input id="studio-${key}-hue" class="studio-hue" type="range" min="0" max="359" step="1">
<div class="studio-hex-row"><label class="studio-sr" for="studio-${key}">${name}</label><input id="studio-${key}" type="color"><label class="studio-sr" for="studio-${key}-hex">${name} hex code</label><input id="studio-${key}-hex" type="text" maxlength="7" pattern="#[0-9a-fA-F]{6}" aria-label="${name} hex code"></div>
<div class="studio-rgb-numbers">${['R','G','B'].map((channel,i) => `<label for="studio-${key}-rgb-${i}">${channel}<input id="studio-${key}-rgb-${i}" type="number" required min="0" max="255" step="1" aria-label="${name} ${channel}"></label>`).join('')}</div>
</div>
<div class="studio-white-picker" hidden>
<div class="studio-white-preview" aria-hidden="true">White preview</div>
<div class="studio-white-blend"><label class="studio-picker-label" for="studio-${key}-blend">White tone</label><input id="studio-${key}-blend" class="studio-white-tone" type="range" min="0" max="100" step="1"><div class="studio-picker-ends"><span>Warm</span><span>Cool</span></div><label class="studio-picker-label" for="studio-${key}-kelvin">Temperature (K)</label><input id="studio-${key}-kelvin" type="number" required min="2000" max="6500" step="1"></div>
<label class="studio-picker-label" for="studio-${key}-level">White level <output id="studio-${key}-level-value"></output></label><input id="studio-${key}-level" type="range" min="0" max="255" step="1">
<p class="meta studio-white-help"></p>
</div><p class="meta studio-mixed" hidden>Mix combines RGB and white. RGB and White select a single source.</p>`;
        el('colours').append(panel);
        const find = suffix => el(key + suffix);
        const square = find('-spectrum'), hue = find('-hue'), native = find(''), hex = find('-hex');
        const level = find('-level'), blend = find('-blend');
        const canEdit = () => !busy && !current()?.locked && current()?.detailsLoaded !== false;
        let hsv = rgbToHsv(memory.rgb), dragging = false;
        function paint() {
            const white = memory.mode === 'white', mixed = memory.mode === 'mixed';
            panel.querySelector('.studio-rgb-picker').hidden = white;
            panel.querySelector('.studio-white-picker').hidden = !white && !mixed;
            find('-rgb-mode').setAttribute('aria-pressed', String(!white && !mixed));
            find('-mixed-mode').setAttribute('aria-pressed', String(mixed));
            find('-white-mode').setAttribute('aria-pressed', String(white));
            square.style.backgroundColor = `hsl(${hsv[0]} 100% 50%)`;
            square.querySelector('span').style.left = hsv[1] * 100 + '%';
            square.querySelector('span').style.top = (1 - hsv[2]) * 100 + '%';
            square.setAttribute('aria-description', `Saturation ${Math.round(hsv[1] * 100)}%, value ${Math.round(hsv[2] * 100)}%`);
            native.value = hex.value = memory.rgb;
            hue.value = Math.round(hsv[0]);
            memory.rgb.match(/[0-9a-f]{2}/gi).forEach((v,i) => { find('-rgb-' + i).value = parseInt(v,16); });
            find('-kelvin').value = Math.round(2000 + memory.blend * 45);
            level.value = memory.level;
            blend.value = memory.blend;
            find('-level-value').textContent = Math.round(memory.level * 100 / 255) + '%';
            blend.setAttribute('aria-valuetext', `${100 - memory.blend}% warm, ${memory.blend}% cool`);
            panel.querySelector('.studio-white-blend').hidden = state.channels === 4;
            panel.querySelector('.studio-white-help').textContent = state.channels === 3 ? 'Temperature is approximated using RGB channels.' : state.channels === 4 ? 'Uses the strip’s white channel.' : 'Uses the warm and cool white channels.';
            // The tone slider keeps its full gradient; the preview reflects white output level.
            blend.style.setProperty('--studio-white-gradient', 'linear-gradient(to right in srgb,' + [0,25,50,75,100].map(v => whiteTone(v) + ' ' + v + '%').join(',') + ')');
            const tone = state.channels !== 4 ? kelvinRgb(2000 + memory.blend * 45) : [255,255,255];
            const sample = 'rgb(' + tone.map(channel => Math.round(channel * memory.level / 255)).join(',') + ')';
            panel.querySelector('.studio-white-preview').style.background = sample;
            panel.querySelector('.studio-white-preview').style.color = memory.level < 140 ? '#fff' : '#333';
            panel.querySelector('.studio-swatch').style.background = white ? sample : memory.rgb;
        }
        function send() {
            let value = { rgb: memory.rgb, ww: 0, cw: 0 };
            if (memory.mode === 'white' || memory.mode === 'mixed') {
                if (state.channels === 3) {
                    const base = memory.mode === 'mixed' ? memory.rgb.match(/[0-9a-f]{2}/gi).map(v => parseInt(v,16)) : [0,0,0];
                    const tone = kelvinRgb(2000 + memory.blend * 45), level = memory.level / 255;
                    value.rgb = '#' + base.map((v,i) => Math.round(memory.mode === 'mixed' ? v * (1-level) + tone[i]*level : tone[i]*level).toString(16).padStart(2,'0')).join('');
                }
                else {
                    value.rgb = memory.mode === 'mixed' ? memory.rgb : '#000000';
                    value.cw = state.channels === 5 ? Math.round(memory.level * memory.blend / 100) : 0;
                    value.ww = memory.level - value.cw;
                }
            }
            memory.lastSent = clone(value);
            return command(key + 'Channels', value);
        }
        for (const mode of ['rgb', 'white', 'mixed']) find('-' + mode + '-mode').addEventListener('click', () => {
            if (!canEdit() || memory.mode === mode) return;
            memory.mode = mode;
            paint();
            send();
        });
        function adoptRgb(value) {
            memory.rgb = value.toLowerCase();
            const next = rgbToHsv(memory.rgb);
            if (next[1] === 0) next[0] = hsv[0];
            if (next[2] === 0) next[1] = hsv[1];
            hsv = next;
        }
        function setRgb(value) {
            adoptRgb(value);
            paint();
        }
        for (let i = 0; i < 3; ++i) find('-rgb-' + i).addEventListener('change', () => {
            const values = [0,1,2].map(n => Number(find('-rgb-' + n).value));
            if (values.some(v => !Number.isInteger(v) || v < 0 || v > 255)) { find('-rgb-' + i).reportValidity(); return; }
            setRgb('#' + values.map(v => v.toString(16).padStart(2,'0')).join('')); send();
        });
        find('-kelvin').addEventListener('change', () => {
            const input = find('-kelvin');
            if (!input.checkValidity() || !Number.isInteger(Number(input.value))) { input.reportValidity(); return; }
            memory.blend = (Number(input.value)-2000)/45; paint(); send();
        });
        native.addEventListener('input', () => setRgb(native.value));
        native.addEventListener('change', send);
        hex.addEventListener('change', () => {
            if (!/^#[0-9a-f]{6}$/i.test(hex.value)) { hex.setCustomValidity('Enter a colour as #RRGGBB'); hex.reportValidity(); return; }
            hex.setCustomValidity(''); setRgb(hex.value); send();
        });
        hex.addEventListener('input', () => hex.setCustomValidity(''));
        function setHsv() { memory.rgb = hsvToRgb(...hsv); paint(); }
        hue.addEventListener('input', () => { hsv[0] = Number(hue.value); setHsv(); });
        hue.addEventListener('change', send);
        function point(event) {
            const bounds = square.getBoundingClientRect();
            hsv[1] = Math.max(0, Math.min(1, (event.clientX - bounds.left) / bounds.width));
            hsv[2] = 1 - Math.max(0, Math.min(1, (event.clientY - bounds.top) / bounds.height));
            setHsv();
        }
        square.addEventListener('pointerdown', event => {
            if (!canEdit() || event.button !== 0) return;
            event.preventDefault(); square.focus(); dragging = true; square.setPointerCapture(event.pointerId); point(event);
        });
        square.addEventListener('pointermove', event => { if (dragging) point(event); });
        square.addEventListener('pointerup', () => { if (dragging) { dragging = false; send(); } });
        square.addEventListener('pointercancel', () => { dragging = false; });
        square.addEventListener('keydown', event => {
            if (!canEdit() || !['ArrowLeft', 'ArrowRight', 'ArrowUp', 'ArrowDown'].includes(event.key)) return;
            event.preventDefault();
            const step = event.shiftKey ? 0.1 : 0.01;
            if (event.key === 'ArrowLeft' || event.key === 'ArrowRight') hsv[1] = Math.max(0, Math.min(1, hsv[1] + (event.key === 'ArrowRight' ? step : -step)));
            else hsv[2] = Math.max(0, Math.min(1, hsv[2] + (event.key === 'ArrowUp' ? step : -step)));
            setHsv();
        });
        square.addEventListener('keyup', event => { if (canEdit() && event.key.startsWith('Arrow')) send(); });
        for (const input of [level, blend]) {
            input.addEventListener('input', () => { memory.level = Number(level.value); memory.blend = Number(blend.value); paint(); });
            input.addEventListener('change', send);
        }
        pickers.set(key, {
            sync(colour, force) {
                if (dragging || (!force && panel.contains(document.activeElement))) return;
                panel.querySelector('.studio-mixed').hidden = colour.rgb === '#000000' || colour.ww + colour.cw === 0;
                // Device channels cannot encode the picker mode (especially RGB strips or zero output).
                // Keep the selected mode and unscaled inputs when our own edit is echoed back.
                if (memory.lastSent && ['rgb','ww','cw'].every(channel => memory.lastSent[channel] === colour[channel])) { paint(); return; }
                if (memory.mode === 'rgb' || memory.mode === 'mixed') adoptRgb(colour.rgb);
                if (memory.mode !== 'rgb' && state.channels === 3) memory.level = parseInt(colour.rgb.slice(1, 3), 16);
                else if (memory.mode !== 'rgb') {
                    memory.level = Math.min(255, colour.ww + colour.cw);
                    if (colour.ww + colour.cw) memory.blend = Math.round(100 * colour.cw / (colour.ww + colour.cw));
                }
                paint();
            }
        });
        paint();
    }
    function render(force = false) {
        const segment = current();
        if (!state.ready) {
            el('form').hidden = true;
            status('Waiting for the device to finish startup…');
            return;
        }
        el('form').hidden = !segment;
        if (!segment) { status('No configured segments are available.'); return; }
        const notes = [segment.detailsLoaded === false ? 'Loading segment details…' : segment.power ? 'Output on' : 'Output off — changes will be visible when switched on'];
        if (!state.globalPower) notes.push('Global power is off');
        if (segment.locked) notes.push('Locked by KNX');
        if (segment.em) notes.push('Effect manager running; changing colours or effects takes manual control');
        el('segment-state').textContent = notes.join(' · ');
        const nextSignature = JSON.stringify([selected, segment.effect, state.channels, segment.parameters.map(p => [p.id, p.type, p.min, p.max, p.options])]);
        // Preserve focused inputs while polling, including unfinished text edits.
        if (signature !== nextSignature) {
            signature = nextSignature;
            el('colours').replaceChildren();
            pickers.clear();
            for (const [key, name] of [['primary', 'Primary colour'], ['secondary', 'Secondary colour']]) colourPicker(key, name, segment[key]);
            el('parameters').replaceChildren();
            if (!segment.parameters.length) {
                const empty = document.createElement('p');
                empty.className = 'meta';
                empty.textContent = 'This effect has no additional parameters.';
                el('parameters').append(empty);
            }
            for (const p of segment.parameters) {
                const send = value => command('parameter', value, { parameter: p.id, effect: segment.effect });
                if (p.type === 'enum' && p.options.length) {
                    const input = document.createElement('select');
                    input.id = 'studio-param-' + p.id;
                    p.options.forEach((name, index) => option(input, index, name));
                    input.value = p.value;
                    input.addEventListener('change', () => send(Number(input.value)));
                    row(el('parameters'), p.name, input);
                } else control(el('parameters'), p.name, 'param-' + p.id, p.type === 'bool' ? 'checkbox' : p.type === 'text' ? 'text' : 'range', p.value, send, p.min, p.max);
            }
        }
        const set = (id, value, checked = false) => {
            const input = el(id);
            if (!input || (!force && input === document.activeElement)) return;
            if (checked) input.checked = Boolean(value);
            else input.value = value;
            const numeric = document.getElementById(input.id + '-number');
            if (numeric && (force || numeric !== document.activeElement)) numeric.value = value;
            const output = input.parentElement.querySelector('output');
            if (output) output.textContent = value;
        };
        set('effect', segment.effect);
        set('brightness', segment.brightness);
        set('power', segment.ownPower ?? segment.power, true);
        el('brightness-value').textContent = Math.round(segment.brightness * 100 / 255) + '%';
        for (const key of ['primary', 'secondary']) pickers.get(key)?.sync(segment[key], force);
        for (const p of segment.parameters) set('param-' + p.id, p.value, p.type === 'bool');
        renderScenes(segment);
        el('draft').textContent = dirty.has(selected) ? 'Draft has pending changes for this segment.' : 'Draft matches the device.';
        el('payload').textContent = buildBatch().map(item => {
            if (item.action === 'effect') return 'Effect: ' + (effects.find(e => e.id === item.value)?.name || item.value);
            if (item.action === 'brightness') return 'Brightness: ' + Math.round(item.value * 100 / 255) + '%';
            if (item.action === 'power') return 'Power: ' + (item.value ? 'On' : 'Off');
            if (item.action === 'primaryChannels' || item.action === 'secondaryChannels') return (item.action === 'primaryChannels' ? 'Primary: ' : 'Secondary: ') + item.value.rgb + ' · white ' + item.value.ww + ' · cool white ' + item.value.cw;
            if (item.action === 'parameter') {
                const parameter = segment.parameters.find(p => p.id === item.parameter);
                return (parameter?.name || 'Parameter') + ': ' + (parameter?.type === 'enum' ? parameter.options[item.value] : parameter?.type === 'bool' ? item.value ? 'On' : 'Off' : item.value);
            }
            return ({sceneRecall:'Recall scene ',sceneStore:'Store scene ',sceneAdd:'Add scene',sceneDelete:'Delete scene '})[item.action] + (item.value ?? '');
        }).join('\n') || 'No pending changes.';
        disabled();
    }
    const visualDirty = new Set();
    function command(action, value, extra = {}) {
        if (busy || current()?.locked || current()?.detailsLoaded === false) return;
        const segment = current();
        if (!segment) return;
        if (action.startsWith('scene')) {
            if (sceneActions.get(selected)?.action === 'sceneRecall' && (action === 'sceneStore' || action === 'sceneAdd')) visualDirty.add(selected);
            sceneActions.set(selected, { segment:selected, action, value });
            if (action === 'sceneRecall') {
                const scene = segment.scenes?.find(s => s.id === value);
                if (scene && scene.effect < 200) {
                    segment.effect = scene.effect;
                    segment.primary = clone(scene.primary); segment.secondary = clone(scene.secondary);
                    segment.brightness = scene.brightness; segment.ownPower = scene.brightness !== 0;
                    segment.parameters = clone(effects.find(e => e.id === scene.effect)?.parameters || []);
                    segment.parameters.forEach(p => { p.value = p.type === 'text' ? scene.text : scene.slots[p.id] ?? p.value; });
                    pickerMemory.clear(); signature = '';
                }
            }
        } else {
            if (sceneActions.get(selected)?.action === 'sceneRecall') sceneActions.delete(selected);
            visualDirty.add(selected);
            if (action === 'effect') {
                segment.effect = value;
                segment.parameters = clone(effects.find(e => e.id === value)?.parameters || []);
                signature = '';
            } else if (action === 'primaryChannels' || action === 'secondaryChannels') segment[action === 'primaryChannels' ? 'primary' : 'secondary'] = clone(value);
            else if (action === 'parameter') {
                const parameter = segment.parameters.find(p => p.id === extra.parameter);
                if (parameter) parameter.value = value;
            } else if (action === 'power') segment.ownPower = Boolean(value);
            else if (action === 'brightness') segment.brightness = value;
        }
        dirty.add(selected);
        render();
        if (autoApply) sendCommand('batch', buildBatch());
        else status('Draft ready · Apply live or Save to device');
    }
    function buildBatch() {
        const segment = current();
        if (!segment) return [];
        const item = (action, value, extra = {}) => ({ segment:selected, action, value, ...extra });
        const commands = [];
        if (visualDirty.has(selected)) {
            commands.push(item('effect', segment.effect), item('primaryChannels', segment.primary), item('secondaryChannels', segment.secondary), item('brightness', segment.brightness));
            for (const p of segment.parameters) commands.push(item('parameter', p.value, { effect:segment.effect, parameter:p.id }));
            commands.push(item('power', Number(segment.ownPower ?? segment.power)));
        }
        if (sceneActions.has(selected)) commands.push(clone(sceneActions.get(selected)));
        return commands;
    }
    function renderScenes(segment) {
        const list = el('scenes');
        list.replaceChildren();
        if (segment.detailsLoaded === false) { list.textContent = 'Loading scenes…'; return; }
        const scenes = segment.scenes || [];
        if (!scenes.length) {
            const empty = document.createElement('p'); empty.className = 'meta'; empty.textContent = 'No scenes configured.'; list.append(empty);
        }
        for (const scene of scenes) {
            const entry = document.createElement('div'); entry.className = 'studio-scene';
            const title = document.createElement('strong');
            title.textContent = 'Scene ' + scene.id + ' · ' + (effects.find(e => e.id === scene.effect)?.name || (scene.effect >= 200 ? 'Effect manager ' + (scene.effect - 199) : 'Effect ' + scene.effect)) + (segment.activeScene === scene.id ? ' · active' : '');
            entry.append(title);
            for (const [label, action] of [['Recall','sceneRecall'],['Store draft here','sceneStore'],['Delete','sceneDelete']]) {
                const button = document.createElement('button'); button.type = 'button'; button.textContent = label;
                button.setAttribute('aria-label', label + ' scene ' + scene.id);
                button.disabled = action === 'sceneDelete' && scene.id !== scenes.length;
                button.addEventListener('click', () => command(action, scene.id)); entry.append(button);
            }
            list.append(entry);
        }
        const add = document.createElement('button'); add.type = 'button'; add.textContent = 'Add scene from draft'; add.disabled = scenes.length >= 10;
        add.addEventListener('click', () => command('sceneAdd', null)); list.append(add);
        const pending = sceneActions.get(selected);
        if (pending) {
            const note = document.createElement('p'); note.className = 'meta'; note.style.marginTop = '10px';
            note.textContent = 'Pending: ' + ({sceneAdd:'add scene',sceneDelete:'delete scene '+pending.value,sceneStore:'store scene '+pending.value,sceneRecall:'recall scene '+pending.value})[pending.action];
            list.append(note);
        }
    }
    function accept(next) {
        liveState = clone(next);
        if (state) next.segments = next.segments.map(segment => dirty.has(segment.id) ? { ...state.segments.find(s => s.id === segment.id), locked:segment.locked, em:segment.em } : segment);
        state = next;
        const ids = state.segments.map(segment => String(segment.id));
        if (JSON.stringify([...el('segment').options].map(item => item.value)) !== JSON.stringify(ids)) {
            el('segment').replaceChildren();
            state.segments.forEach(segment => option(el('segment'), segment.id, 'Segment ' + segment.id));
        }
        if (!state.segments.some(segment => segment.id === selected)) selected = state.segments[0]?.id || 0;
        el('segment').value = selected;
        render(busy);
    }
    async function sendCommand(action, value, extra = {}) {
        if (busy) return;
        busy = true;
        clearTimeout(timer);
        disabled();
        status('Applying…');
        try {
            const queued = await request('control', {
                method: 'POST', headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ segment: selected, action, value, ...extra })
            });
            const deadline = Date.now() + 8000;
            while (true) {
                await new Promise(resolve => setTimeout(resolve, 150));
                const next = await request('state?segment=' + Math.max(selected, 1));
                if (next.completedId >= queued.id) {
                    if (!next.error && (action === 'batch' || action === 'load')) { dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); if (action === 'load') pickerMemory.clear(); signature = ''; }
                    accept(next);
                    if (next.completedId === queued.id && next.error) throw new Error(next.error);
                    const message = action === 'save' ? 'Saved to device · import this segment’s saved Studio settings in ETS' : action === 'load' ? 'Loaded current device values' : 'Applied';
                    status(next.completedId === queued.id ? message : 'Device state refreshed; another browser also changed it');
                    return true;
                }
                if (Date.now() > deadline) throw new Error('Device did not confirm the change. Refresh to check its state.');
            }
        } catch (error) {
            signature = '';
            render(true);
            status(error.message, true);
            return false;
        } finally {
            busy = false;
            disabled();
            timer = setTimeout(poll, 1200);
        }
    }
    async function poll() {
        if (busy) return;
        try {
            if (!effects.length) {
                effects = await request('effects');
                el('effect').replaceChildren();
                effects.forEach(effect => option(el('effect'), effect.id, effect.name));
            }
            accept(await request('state?segment=' + Math.max(selected, 1)));
            if (state.ready && state.segments.length) status(dirty.has(selected) ? 'Draft ready · Apply live or Save to device' : 'Connected');
        } catch (error) {
            el('controls').disabled = true;
            status('Connection lost · ' + error.message, true);
        } finally { if (!busy) timer = setTimeout(poll, document.hidden ? 5000 : 1200); }
    }
    el('auto-apply').addEventListener('click', () => {
        autoApply = !autoApply;
        el('auto-apply').setAttribute('aria-pressed', String(autoApply));
        el('auto-apply').textContent = 'Automatic apply: ' + (autoApply ? 'On' : 'Off');
        if (autoApply && dirty.has(selected) && !busy) sendCommand('batch', buildBatch());
        else if (!busy) status(autoApply ? 'Automatic apply enabled' : 'Automatic apply disabled · edit a draft, then Apply live');
    });
    el('apply').addEventListener('click', () => sendCommand('batch', buildBatch()));
    el('reset').addEventListener('click', () => {
        dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); pickerMemory.clear(); signature = '';
        state.segments = state.segments.map(s => s.id === selected ? clone(liveState.segments.find(live => live.id === selected)) : s); render(true); status('Draft reset');
    });
    el('save').addEventListener('click', async () => {
        if (dirty.has(selected) && !await sendCommand('batch', buildBatch())) return;
        sendCommand('save', null);
    });
    el('load').addEventListener('click', () => sendCommand('load', null));
    el('form').addEventListener('submit', event => event.preventDefault());
    el('segment').addEventListener('change', () => { selected = Number(el('segment').value); signature = ''; render(); clearTimeout(timer); poll(); });
    el('effect').addEventListener('change', () => command('effect', Number(el('effect').value)));
    el('brightness').addEventListener('input', () => { el('brightness-value').textContent = Math.round(Number(el('brightness').value) * 100 / 255) + '%'; });
    el('power').addEventListener('change', () => command('power', Number(el('power').checked)));
    el('brightness').addEventListener('change', () => command('brightness', Number(el('brightness').value)));
    poll();
})();
)JS";
