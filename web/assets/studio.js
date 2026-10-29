// studio.js - NeoPixel Studio /neopixel: live control, scenes and commissioning of one segment.
// The page polls /neopixel/api/state; changes are kept as a draft per segment and sent as one batch.
(() => {
    'use strict';
    const root = document.getElementById('neopixel-studio');
    if (!root) return;
    const el = id => document.getElementById('studio-' + id);
    const clone = value => JSON.parse(JSON.stringify(value));
    const COLOURS = [['primary', 'Farbe 1'], ['secondary', 'Farbe 2']];

    // State: device snapshot (liveState), draft view of it (state) and per-segment draft bookkeeping.
    let state, liveState, effects = [], selected = 0, busy = false, autoApply = true, signature = '', timer, hint = 'info', hold = 0;
    // UI socket (base.js): the device pushes "studio:<rev>" on every state change; HTTP polling is only the fallback.
    const pushed = () => !!(window.OpenKNX && window.OpenKNX.connected);
    let polling = false, again = false, notify = null, changed = false, failed = false;
    // A push that arrives while no one waits (POST still running, state GET in flight) is kept in 'changed'.
    const waitChange = ms => new Promise(resolve => { if (changed) { changed = false; resolve(); return; } const t = setTimeout(() => { notify = null; resolve(); }, ms); notify = () => { clearTimeout(t); notify = null; resolve(); }; });
    const dirty = new Set(), visualDirty = new Set(), sceneActions = new Map();
    const pickerMemory = new Map(), pickers = new Map();
    const commissioningDrafts = new Map(), commissioningDirty = new Set(), commissioningCorrections = new Map();
    let commissioningSignature = '', commissioningTimer;
    const current = () => state?.segments.find(segment => segment.id === selected);

    // Helpers.
    function levelPercent(value) {
        const percent = Number(value) * 100 / 255, rounded = Math.round(percent);
        return (percent > 0 && percent < 100 && (rounded === 0 || rounded === 100) ? percent.toFixed(1).replace('.', ',') : rounded) + ' %';
    }
    function paintRange(input) {
        if (input.matches('.studio-hue,.studio-white-tone')) return;
        const min = Number(input.min || 0), max = Number(input.max || 100);
        const percent = max > min ? Math.max(0, Math.min(100, (Number(input.value) - min) * 100 / (max - min))) : 0;
        input.style.setProperty('--studio-fill', percent + '%');
    }
    root.addEventListener('input', event => {
        if (event.target.matches('input[type=range]')) paintRange(event.target);
    });
    // Messages go to the "Hinweise" pill of the band; kind ok/info/warn/bad (true = bad), keep = ms before routine polls may replace it.
    const status = (text, kind = 'ok', keep = 0) => {
        if (kind === true) kind = 'bad';
        hint = kind;
        hold = keep ? Date.now() + keep : 0;
        el('status').className = 'np-pill st-hint ' + kind;
        el('status').lastElementChild.textContent = text;
        band();
    };
    const idle = () => { if (Date.now() >= hold) status('keine'); };
    // Band edge = worst state; mode pill, apply button and draft card follow live/draft mode.
    function band() {
        const segment = current(), drafting = !autoApply && dirty.has(selected);
        const bad = hint === 'bad' || segment?.locked, info = segment?.testing;
        el('bar').className = 'np-bar' + (bad ? ' bad' : drafting || (state && state.ready && !state.globalPower) ? ' warn' : info ? ' info' : '');
        el('mode').className = 'np-pill ' + (autoApply ? 'ok' : drafting ? 'warn' : 'idle');
        el('mode').lastElementChild.textContent = autoApply ? 'Live' : drafting ? 'Entwurf · offen' : 'Entwurf';
        el('apply').hidden = autoApply;
        el('draft').hidden = !drafting;
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
            number.setAttribute('aria-label', name + ' (Zahl)');
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

    // Device API.
    async function request(path, options) {
        const controller = new AbortController();
        const timeout = setTimeout(() => controller.abort(), 6000);
        try {
            const response = await fetch('/neopixel/api/' + path, {
                cache: 'no-store', ...options, signal: controller.signal
            });
            const data = await response.json();
            if (!response.ok) throw new Error(data.error || 'Gerät meldet Fehler (' + response.status + ')');
            return data;
        } finally { clearTimeout(timeout); }
    }
    function disabled() {
        const segment = current(), none = busy || !segment;
        const blocked = none || segment.locked || segment.detailsLoaded === false;
        el('controls').disabled = blocked || segment.testing;
        el('commission-fields').disabled = blocked || segment.testing;
        root.querySelectorAll('[data-test]').forEach(b => { b.disabled = blocked || segment.em || !state.globalPower || (b.dataset.test === 'chase' && !effects.some(e => e.id === 1)) || (b.dataset.test === 'cycle' && !effects.some(e => e.id === 7)); });
        el('test-stop').disabled = busy || !segment?.testing;
        el('test-level').disabled = none;
        el('segment').disabled = busy || state?.segments.some(s => s.testing);
        if (el('segment').disabled && !el('seglist').hidden) openSegList(false);
        el('save').disabled = none || segment.testing;
        el('load').disabled = none;
        el('restore-ets').disabled = blocked;
        el('apply').disabled = none || segment.locked || !dirty.has(selected);
        el('reset').disabled = busy || !dirty.has(selected);
        for (const square of root.querySelectorAll('.studio-spectrum')) {
            square.tabIndex = el('controls').disabled ? -1 : 0;
            square.setAttribute('aria-disabled', String(el('controls').disabled));
        }
    }

    // Colour picker: RGB (spectrum + hue), white (level + tone) or both mixed.
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
        panel.className = 'studio-picker'; panel.dataset.key = key;
        // Keys and names are fixed application strings. Device values are assigned through DOM properties.
        panel.innerHTML = `<div class="studio-picker-heading"><h3>${name}</h3><span class="studio-swatch" aria-hidden="true"></span></div>
<div class="studio-picker-tabs" role="group" aria-label="${name}: Modus">
<button type="button" id="studio-${key}-rgb-mode">RGB</button><button type="button" id="studio-${key}-white-mode">Weiß</button><button type="button" id="studio-${key}-mixed-mode">Mix</button></div>
<div class="studio-rgb-picker">
<div id="studio-${key}-spectrum" class="studio-spectrum" tabindex="0" role="group" aria-label="${name}: Sättigung und Helligkeit. Links und rechts ändern die Sättigung, hoch und runter die Helligkeit."><span class="studio-picker-cursor"></span></div>
<label class="studio-picker-label" for="studio-${key}-hue">Farbton</label><input id="studio-${key}-hue" class="studio-hue" type="range" min="0" max="359" step="1">
<div class="studio-hex-row"><label class="studio-sr" for="studio-${key}">${name}</label><input id="studio-${key}" type="color"><label class="studio-sr" for="studio-${key}-hex">${name} Hex-Code</label><input id="studio-${key}-hex" type="text" maxlength="7" pattern="#[0-9a-fA-F]{6}" aria-label="${name} Hex-Code"></div>
<div class="studio-rgb-numbers">${['R','G','B'].map((channel,i) => `<label for="studio-${key}-rgb-${i}">${channel}<input id="studio-${key}-rgb-${i}" type="number" required min="0" max="255" step="1" aria-label="${name} ${channel}"></label>`).join('')}</div>
</div>
<div class="studio-white-picker" hidden>
<div class="studio-white-preview" aria-hidden="true">Weißvorschau</div>
<div class="studio-white-blend"><label class="studio-picker-label" for="studio-${key}-blend">Weißton</label><input id="studio-${key}-blend" class="studio-white-tone" type="range" min="0" max="100" step="1"><div class="studio-picker-ends"><span>Warm</span><span>Kalt</span></div><label class="studio-picker-label" for="studio-${key}-kelvin">Farbtemperatur (K)</label><input id="studio-${key}-kelvin" type="number" required min="2000" max="6500" step="1"></div>
<label class="studio-picker-label" for="studio-${key}-level">Weißanteil <output id="studio-${key}-level-value"></output></label><input id="studio-${key}-level" type="range" min="0" max="255" step="1">
<p class="meta studio-white-help"></p>
</div><p class="meta studio-mixed" hidden>Mix kombiniert RGB und Weiß. RGB und Weiß nutzen jeweils nur eine Quelle.</p>`;
        el('colours').append(panel);
        const find = suffix => el(key + suffix);
        const square = find('-spectrum'), hue = find('-hue'), native = find(''), hex = find('-hex');
        const level = find('-level'), blend = find('-blend');
        const canEdit = () => !busy && !current()?.locked && current()?.detailsLoaded !== false;
        let hsv = rgbToHsv(memory.rgb), dragging = false;
        function paint() {
            const white = memory.mode === 'white', mixed = memory.mode === 'mixed';
            panel.dataset.mode = memory.mode;
            panel.querySelector('.studio-rgb-picker').hidden = white;
            panel.querySelector('.studio-white-picker').hidden = !white && !mixed;
            find('-rgb-mode').setAttribute('aria-pressed', String(!white && !mixed));
            find('-mixed-mode').setAttribute('aria-pressed', String(mixed));
            find('-white-mode').setAttribute('aria-pressed', String(white));
            square.style.backgroundColor = `hsl(${hsv[0]} 100% 50%)`;
            square.querySelector('span').style.left = hsv[1] * 100 + '%';
            square.querySelector('span').style.top = (1 - hsv[2]) * 100 + '%';
            square.setAttribute('aria-description', `Sättigung ${Math.round(hsv[1] * 100)} %, Helligkeit ${Math.round(hsv[2] * 100)} %`);
            native.value = hex.value = memory.rgb;
            hue.value = Math.round(hsv[0]);
            memory.rgb.match(/[0-9a-f]{2}/gi).forEach((v,i) => { find('-rgb-' + i).value = parseInt(v,16); });
            find('-kelvin').value = Math.round(2000 + memory.blend * 45);
            level.value = memory.level;
            blend.value = memory.blend;
            find('-level-value').textContent = levelPercent(memory.level);
            blend.setAttribute('aria-valuetext', `${100 - memory.blend} % warm, ${memory.blend} % kalt`);
            panel.querySelector('.studio-white-blend').hidden = state.channels === 4;
            panel.querySelector('.studio-white-help').textContent = state.channels === 3 ? 'Die Farbtemperatur wird über die RGB-Kanäle angenähert.' : state.channels === 4 ? 'Nutzt den Weißkanal des Streifens.' : 'Nutzt die Warm- und Kaltweißkanäle.';
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
            if (!/^#[0-9a-f]{6}$/i.test(hex.value)) { hex.setCustomValidity('Farbe als #RRGGBB eingeben'); hex.reportValidity(); return; }
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

    // Render the selected segment; inputs are rebuilt only when the effect or its parameter set changes.
    function render(force = false) {
        root.dataset.ready = String(state.ready);
        const segment = current();
        if (!state.ready) {
            el('form').hidden = true;
            if (state.configured === false && !state.programming) status('Das Gerät ist noch nicht konfiguriert. Erst nach dem Laden der Gerätekonfiguration sind Segmente angelegt.', 'warn');
            else status(state.programming ? 'Die Gerätekonfiguration wird geladen. Studio ist nach dem Neustart des Geräts verfügbar.' : 'Warte auf den Start des Geräts …', 'info');
            return;
        }
        el('form').hidden = !segment;
        if (!segment) { status('Keine Segmente konfiguriert.', 'warn'); return; }
        // The band shows the device, the inputs show the draft.
        const live = liveState?.segments.find(s => s.id === selected) || segment;
        const [cls, text, tip] = segState(live);
        el('state').className = 'np-pill ' + cls; el('state').lastElementChild.textContent = text; el('state').title = tip;
        el('b-num').className = 'np-badge t' + (selected % 8); // same tone as the segment on /neo
        el('b-num').textContent = selected;
        el('b-effect').textContent = effects.find(e => e.id === live.effect)?.name ?? live.effect;
        el('b-bri').textContent = levelPercent(live.brightness);
        el('b-scene').textContent = live.activeScene || '–';
        el('b-em').hidden = !live.em;
        const nextSignature = JSON.stringify([selected, segment.effect, state.channels, segment.parameters.map(p => [p.id, p.type, p.min, p.max, p.options])]);
        // Preserve focused inputs while polling, including unfinished text edits.
        if (signature !== nextSignature) {
            signature = nextSignature;
            el('colours').replaceChildren();
            pickers.clear();
            for (const [key, name] of COLOURS) colourPicker(key, name, segment[key]);
            el('parameters').replaceChildren();
            if (!segment.parameters.length) {
                const empty = document.createElement('p');
                empty.className = 'meta';
                empty.textContent = 'Dieser Effekt hat keine weiteren Parameter.';
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
        el('brightness-value').textContent = levelPercent(segment.brightness);
        for (const [key] of COLOURS) pickers.get(key)?.sync(segment[key], force);
        for (const p of segment.parameters) set('param-' + p.id, p.value, p.type === 'bool');
        renderScenes(segment);
        renderCommissioning(segment);
        root.querySelectorAll('input[type=range]').forEach(paintRange);
        renderDraft(segment, live);
        disabled();
        band();
    }
    // State pill of a segment as the device runs it: [class, text, tooltip]. Details (parameters, scenes) only matter for the selected one.
    function segState(s, list = false) {
        return !list && s.detailsLoaded === false ? ['idle', 'lädt …', ''] : s.locked ? ['bad', 'Gesperrt', 'Über KNX gesperrt'] : s.testing ? ['info', 'Test', ''] : !state.globalPower ? ['warn', 'Global aus', 'Global ausgeschaltet'] : s.power ? ['ok', 'Ein', ''] : ['idle', 'Aus', 'Änderungen werden nach dem Einschalten sichtbar'];
    }
    // Segment list of the picker: plate, name, effect and state pill per segment, from the device values.
    function renderSegList() {
        const list = el('seglist'), source = (liveState || state)?.segments || [];
        if (list.hidden || !state) return;
        // Rebuild only when something shown changed: a rebuild drops keyboard focus and can swallow a click.
        const sig = JSON.stringify([selected, state.globalPower, source.map(s => [s.id, s.effect, segState(s, true)[1]]), effects.length]);
        if (list._sig === sig) return;
        list._sig = sig;
        list.replaceChildren(...(liveState || state).segments.map(s => {
            const item = document.createElement('button'), plate = document.createElement('b'), name = document.createElement('span'), fx = document.createElement('em'), pill = document.createElement('span'), [cls, text] = segState(s, true);
            item.type = 'button'; item.className = 'st-sp-it'; item.setAttribute('role', 'option'); item.setAttribute('aria-selected', String(s.id === selected)); item.dataset.seg = s.id;
            plate.className = 'np-badge t' + (s.id % 8); plate.textContent = s.id;
            name.textContent = 'Segment ' + s.id;
            fx.textContent = effects.find(e => e.id === s.effect)?.name ?? '';
            pill.className = 'np-pill ' + cls; pill.innerHTML = cls === 'idle' ? '' : '<i class="np-dot"></i>'; pill.append(text);
            item.append(plate, name, fx, pill);
            return item;
        }));
    }
    function openSegList(open) {
        el('seglist').hidden = !open;
        el('seglist')._sig = '';
        el('segment').setAttribute('aria-expanded', String(open));
        renderSegList();
    }
    // Draft card: device value -> draft value for every changed field, plus the pending scene action.
    function renderDraft(segment, live) {
        const rows = [], name = id => effects.find(e => e.id === id)?.name ?? id;
        const colour = c => c ? c.rgb + (c.ww || c.cw ? ' · Weiß ' + c.ww + ' · Kaltweiß ' + c.cw : '') : '–';
        const onOff = s => (s.ownPower ?? s.power) ? 'Ein' : 'Aus';
        const add = (label, from, to) => { if (String(from) !== String(to)) rows.push([label, from, to]); };
        if (visualDirty.has(selected)) {
            add('Effekt', name(live.effect), name(segment.effect));
            add('Helligkeit', levelPercent(live.brightness), levelPercent(segment.brightness));
            add('Ein/Aus', onOff(live), onOff(segment));
            add('Farbe 1', colour(live.primary), colour(segment.primary));
            add('Farbe 2', colour(live.secondary), colour(segment.secondary));
            if (live.effect === segment.effect) for (const p of segment.parameters) {
                const fmt = v => p.type === 'enum' ? p.options[v] : p.type === 'bool' ? v ? 'Ein' : 'Aus' : v;
                const old = live.parameters?.find(q => q.id === p.id);
                add(p.name, old ? fmt(old.value) : '–', fmt(p.value));
            }
        }
        const scene = sceneActions.get(selected);
        if (scene) rows.push(['Szene', '', ({sceneRecall:'Szene ' + scene.value + ' abrufen',sceneStore:'Entwurf als Szene ' + scene.value + ' speichern',sceneAdd:'Szene aus Entwurf hinzufügen',sceneDelete:'Szene ' + scene.value + ' löschen'})[scene.action]]);
        el('payload').replaceChildren(...(rows.length ? rows : [['', '', 'Keine Abweichung vom Gerät']]).map(([label, from, to]) => {
            const item = document.createElement('li'), key = document.createElement('i');
            key.textContent = label;
            item.append(key);
            if (from !== '') { const old = document.createElement('s'); old.textContent = from; item.append(old, ' → '); }
            item.append(String(to));
            return item;
        }));
    }

    // Draft and batch: every edit updates the draft; auto apply sends the whole batch of the segment.
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

    // Scenes.
    function renderScenes(segment) {
        const list = el('scenes');
        list.replaceChildren();
        if (segment.detailsLoaded === false) { list.textContent = 'Szenen werden geladen …'; return; }
        const scenes = segment.scenes || [];
        if (!scenes.length) {
            const empty = document.createElement('p'); empty.className = 'meta'; empty.textContent = 'Keine Szenen konfiguriert.'; list.append(empty);
        }
        for (const scene of scenes) {
            const entry = document.createElement('div'); entry.className = 'studio-scene';
            const title = document.createElement('strong');
            title.textContent = 'Szene ' + scene.id + ' · ' + (effects.find(e => e.id === scene.effect)?.name || (scene.effect >= 200 ? 'Effektmanager ' + (scene.effect - 199) : 'Effekt ' + scene.effect)) + (segment.activeScene === scene.id ? ' · aktiv' : '');
            entry.append(title);
            for (const [label, action] of [['Abrufen','sceneRecall'],['Entwurf hier speichern','sceneStore'],['Löschen','sceneDelete']]) {
                const button = document.createElement('button'); button.type = 'button'; button.textContent = label;
                button.setAttribute('aria-label', label + ' – Szene ' + scene.id);
                button.disabled = action === 'sceneDelete' && scene.id !== scenes.length;
                button.addEventListener('click', () => command(action, scene.id)); entry.append(button);
            }
            list.append(entry);
        }
        const add = document.createElement('button'); add.type = 'button'; add.textContent = 'Szene aus Entwurf hinzufügen'; add.disabled = scenes.length >= 10;
        add.addEventListener('click', () => command('sceneAdd', null)); list.append(add);
        const pending = sceneActions.get(selected);
        if (pending) {
            const note = document.createElement('p'); note.className = 'meta'; note.style.marginTop = '10px';
            note.textContent = 'Ausstehend: ' + ({sceneAdd:'Szene hinzufügen',sceneDelete:'Szene '+pending.value+' löschen',sceneStore:'Szene '+pending.value+' speichern',sceneRecall:'Szene '+pending.value+' abrufen'})[pending.action];
            list.append(note);
        }
    }

    // Polling and command round trip: POST queues, the state poll confirms by completedId.
    function accept(next) {
        liveState = clone(next);
        if (state) next.segments = next.segments.map(segment => dirty.has(segment.id) ? { ...state.segments.find(s => s.id === segment.id), locked:segment.locked, em:segment.em, commissioning:segment.commissioning, commissioningPending:segment.commissioningPending, commissioningApplied:segment.commissioningApplied, testing:segment.testing } : segment);
        state = next;
        if (!state.segments.some(segment => segment.id === selected)) selected = state.segments[0]?.id || 0;
        // A running test locks the picker: show its segment, so Stop is reachable (e.g. after a reload).
        const testing = state.segments.find(segment => segment.testing);
        if (testing && testing.id !== selected) { selected = testing.id; signature = ''; }
        render(busy);
        renderSegList();
    }
    const DONE = {
        save: 'Im Gerät gespeichert · die Studio-Einstellungen dieses Segments jetzt in die Gerätekonfiguration übernehmen',
        testStart: 'Test aktiv · läuft, bis Stop gedrückt oder diese Seite geschlossen wird',
        testStop: 'Test beendet · vorheriger Zustand wiederhergestellt',
        commissioning: 'LED-Konfiguration live angewendet · für die Gerätekonfiguration „Im Gerät speichern“ drücken',
        load: 'Live-Werte neu geladen',
        restoreEts: 'Gerätekonfiguration wiederhergestellt · gespeicherte Studio-Einstellungen unverändert'
    };
    async function sendCommand(action, value, extra = {}) {
        if (busy) return;
        busy = true;
        clearTimeout(timer);
        disabled();
        status('Wird übernommen …', 'info');
        try {
            const queued = await request('control', {
                method: 'POST', headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ segment: selected, action, value, ...extra })
            });
            const deadline = Date.now() + 8000;
            while (true) {
                await waitChange(pushed() ? 300 : 150);
                const next = await request('state?segment=' + Math.max(selected, 1));
                if (next.completedId >= queued.id) {
                    if (!next.error && (action === 'batch' || action === 'load' || action === 'restoreEts' || action === 'testStart' || action === 'testStop')) { dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); if (action === 'load' || action === 'restoreEts' || action === 'testStart' || action === 'testStop') pickerMemory.clear(); signature = ''; }
                    accept(next);
                    if (next.completedId === queued.id && next.error) throw new Error(next.error);
                    if (next.completedId === queued.id) status(DONE[action] || 'Übernommen', 'ok', DONE[action] ? 5000 : 1500);
                    else status('Gerätezustand aktualisiert; ein anderer Browser hat ebenfalls etwas geändert', 'info', 5000);
                    return true;
                }
                if (Date.now() > deadline) throw new Error('Das Gerät hat die Änderung nicht bestätigt. Seite neu laden und den Zustand prüfen.');
            }
        } catch (error) {
            signature = '';
            render(true);
            status(error.message, 'bad', 8000);
            return false;
        } finally {
            busy = false;
            disabled();
            timer = setTimeout(poll, 1200);
            if (levelWanted !== null) setTimeout(applyTestLevel, 0); // a level picked while busy
        }
    }
    async function poll() {
        if (busy) return;
        if (polling) { again = true; return; }
        polling = true;
        clearTimeout(timer);
        try {
            if (!effects.length) {
                effects = await request('effects');
                el('effect').replaceChildren();
                effects.forEach(effect => option(el('effect'), effect.id, effect.name));
            }
            accept(await request('state?segment=' + Math.max(selected, 1)));
            failed = false;
            if (state.ready && state.segments.length) idle();
        } catch (error) {
            el('controls').disabled = true;
            failed = true;
            status('Verbindung verloren · ' + error.message, 'bad', 1);
        } finally {
            polling = false;
            if (again) { again = false; setTimeout(poll, 0); }
            else if (!busy) timer = setTimeout(poll, pushed() && !failed ? 10000 : document.hidden ? 5000 : 1200);
        }
    }
    // base.js is a later deferred script; it has run once DOMContentLoaded fires.
    document.addEventListener('DOMContentLoaded', () => {
        const socket = window.OpenKNX;
        if (!socket) return;
        socket.on('studio', () => { if (notify) notify(); else if (busy) changed = true; else poll(); });
        socket.onState(s => { if (s === 'open') poll(); });
    });
    document.addEventListener('visibilitychange', () => { if (!document.hidden) poll(); });

    // Actions: live/draft mode (menu switch or mode pill), apply, discard, save, menu.
    async function setAutoApply(on) {
        autoApply = on;
        el('auto-apply').checked = on;
        band();
        if (autoApply && !busy) {
            if (!await stageCommissioning()) return;
            if (dirty.has(selected)) await sendCommand('batch', buildBatch());
            else status('Änderungen gelten sofort', 'ok', 3000);
        } else if (!busy) status('Entwurf: Änderungen sammeln, dann übernehmen', 'info', 4000);
    }
    el('auto-apply').addEventListener('change', () => setAutoApply(el('auto-apply').checked));
    el('mode').addEventListener('click', () => setAutoApply(!autoApply));
    el('apply').addEventListener('click', async () => { if (await stageCommissioning()) await sendCommand('batch', buildBatch()); });
    el('reset').addEventListener('click', () => {
        dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); pickerMemory.clear(); signature = '';
        state.segments = state.segments.map(s => s.id === selected ? clone(liveState.segments.find(live => live.id === selected)) : s); render(true); status('Entwurf verworfen', 'ok', 3000);
    });
    el('save').addEventListener('click', async () => {
        const button = el('save');
        let saved = false;
        button.textContent = 'Speichert …';
        try {
            if (!await stageCommissioning()) return;
            if (dirty.has(selected) && !await sendCommand('batch', buildBatch())) return;
            saved = await sendCommand('save', null);
        } finally {
            button.textContent = saved ? 'Gespeichert ✓' : 'Im Gerät speichern';
            if (saved) setTimeout(() => { button.textContent = 'Im Gerät speichern'; }, 2500);
        }
    });
    const more = el('more');
    const closeMenu = () => { more.open = false; };
    el('load').addEventListener('click', () => { closeMenu(); sendCommand('load', null); });
    el('restore-ets').addEventListener('click', () => { closeMenu(); sendCommand('restoreEts', null); });
    document.addEventListener('click', event => {
        if (more.open && !more.contains(event.target)) closeMenu();
        if (!el('seglist').hidden && !el('segment').parentNode.contains(event.target)) openSegList(false);
    });
    document.addEventListener('keydown', event => {
        if (event.key !== 'Escape') return;
        if (more.open) { closeMenu(); more.querySelector('summary').focus(); }
        if (!el('seglist').hidden) { openSegList(false); el('segment').focus(); }
    });
    el('form').addEventListener('submit', event => event.preventDefault());
    el('segment').addEventListener('click', () => openSegList(el('seglist').hidden));
    el('seglist').addEventListener('click', event => {
        const item = event.target.closest('[data-seg]');
        if (!item || busy || el('segment').disabled) return;
        openSegList(false);
        el('segment').focus();
        if (Number(item.dataset.seg) === selected) return;
        selected = Number(item.dataset.seg); signature = ''; render(); clearTimeout(timer); poll();
    });
    el('effect').addEventListener('change', () => command('effect', Number(el('effect').value)));
    el('brightness').addEventListener('input', () => { el('brightness-value').textContent = levelPercent(el('brightness').value); });
    el('power').addEventListener('change', () => command('power', Number(el('power').checked)));
    el('brightness').addEventListener('change', () => command('brightness', Number(el('brightness').value)));

    // Commissioning: strip lengths, skipped LEDs and the segment range as a draft, plus the wiring test.
    const UNSAVED = 'Ungespeicherter Entwurf · mit „Im Gerät speichern“ sichern.';
    const ledTotal = draft => draft.strips.reduce((sum, s) => sum + s.length, 0);
    function scheduleCommissioning(delay = 400) {
        clearTimeout(commissioningTimer);
        if (!autoApply) return;
        const segmentId = selected;
        commissioningTimer = setTimeout(() => {
            if (!autoApply || selected !== segmentId || !commissioningDirty.has(segmentId)) return;
            if (busy) { scheduleCommissioning(); return; }
            stageCommissioning();
        }, delay);
    }
    function renderCommissioning(segment) {
        if (!segment.commissioning) return;
        if (!commissioningDirty.has(selected)) commissioningDrafts.set(selected, clone(segment.commissioning));
        const draft = commissioningDrafts.get(selected);
        const next = JSON.stringify([selected, draft.strips.map(s => s.id)]);
        if (next !== commissioningSignature) {
            commissioningSignature = next;
            el('strip-fields').replaceChildren();
            for (const strip of draft.strips) {
                const entry = document.createElement('div'); entry.className = 'st-strip';
                // Same plate as the strip on /neo: S<n> in tone t((n-1) % 8).
                entry.innerHTML = `<span class="st-sname"><b class="np-badge t${(strip.id - 1) % 8}">S${strip.id}</b>Streifen ${strip.id}</span><input id="studio-strip-${strip.id}-length" type="number" min="0" max="16383" step="1" required aria-label="Streifen ${strip.id}: Anzahl LEDs / Gruppen"><input id="studio-strip-${strip.id}-skip" type="number" min="0" max="255" step="1" required aria-label="Streifen ${strip.id}: erste LEDs überspringen">`;
                el('strip-fields').append(entry);
                for (const key of ['length','skip']) el(`strip-${strip.id}-${key}`).addEventListener('input', event => {
                    const current = commissioningDrafts.get(selected);
                    const oldTotal = ledTotal(current);
                    current.strips.find(s => s.id === strip.id)[key] = Number(event.target.value);
                    const total = ledTotal(current);
                    if (key === 'length' && total > 0) {
                        if (current.end === oldTotal || current.end > total) current.end = Math.min(16383,total);
                        current.start = Math.min(current.start,current.end);
                        el('start-led').value = current.start; el('end-led').value = current.end;
                    }
                    commissioningDirty.add(selected); el('commission-status').textContent = UNSAVED; clampCommissioningEnd(); scheduleCommissioning();
                });
                for (const key of ['length','skip']) el(`strip-${strip.id}-${key}`).addEventListener('change', () => scheduleCommissioning(0));
            }
        }
        for (const strip of draft.strips) for (const key of ['length','skip']) {
            const input = el(`strip-${strip.id}-${key}`);
            if (input !== document.activeElement) input.value = strip[key];
        }
        el('end-led').max = Math.max(1, Math.min(16383, ledTotal(draft)));
        for (const key of ['start','end']) if (el(key + '-led') !== document.activeElement) el(key + '-led').value = draft[key];
        el('c-seg').className = 'np-badge t' + (selected % 8); el('c-seg').textContent = selected; el('c-segname').textContent = 'Segment ' + selected;
        if (!commissioningDirty.has(selected)) el('commission-status').textContent = segment.commissioningApplied === false ? 'Gespeicherter Entwurf · mit „Live anwenden“ aktivieren.' : segment.commissioningPending ? 'Live-Konfiguration weicht von der Gerätekonfiguration ab · für den dauerhaften Startzustand speichern und in die Gerätekonfiguration übernehmen.' : 'Aufteilung aus der Gerätekonfiguration · im Modus Live gelten Änderungen sofort.';
        const [testClass, testText] = segment.testing ? ['info', 'Test aktiv · bis Stop'] : segment.locked ? ['bad', 'KNX-Sperre aktiv'] : !state.globalPower ? ['warn', 'Global ausgeschaltet'] : segment.em ? ['idle', 'Effektmanager läuft'] : ['ok', 'Bereit'];
        el('test-status').className = 'np-pill ' + testClass;
        el('test-status').textContent = testText;
        root.querySelectorAll('[data-test]').forEach(b => b.setAttribute('aria-pressed', String(!!segment.testing && segment.testPattern === b.dataset.test)));
    }
    function clampCommissioningEnd() {
        const draft = commissioningDrafts.get(selected);
        const maximum = Math.min(16383, ledTotal(draft));
        el('end-led').max = Math.max(1, maximum);
        if (Number.isFinite(draft.end) && maximum > 0 && draft.end > maximum) {
            draft.end = maximum;
            draft.start = Math.min(draft.start, maximum);
            el('end-led').value = draft.end; el('start-led').value = draft.start;
            commissioningDirty.add(selected);
            commissioningCorrections.set(selected, maximum);
            el('commission-status').textContent = `Letzte LED auf ${maximum} begrenzt (Summe der LED-Anzahlen).`;
            return maximum;
        }
        return null;
    }
    async function stageCommissioning(force = false) {
        if (!force && !commissioningDirty.has(selected)) return true;
        const clampedMaximum = clampCommissioningEnd() ?? commissioningCorrections.get(selected) ?? null;
        const invalid = [...el('commission-fields').querySelectorAll('input')].find(input => !input.checkValidity());
        if (invalid) { invalid.reportValidity(); status('Ungültige LED-Konfiguration · Eingabebereich prüfen.', 'bad', 8000); return false; }
        const draft = commissioningDrafts.get(selected);
        if (draft.strips.some(s => s.length ? s.skip >= s.length : s.skip !== 0) || draft.start > draft.end || draft.end > ledTotal(draft)) {
            status('LED-Bereich und übersprungene LEDs müssen innerhalb der Streifen liegen.', 'bad', 8000); return false;
        }
        const segmentId = selected;
        const submitted = JSON.stringify(draft);
        const result = await sendCommand('commissioning', clone(draft));
        if (result && JSON.stringify(commissioningDrafts.get(segmentId)) === submitted) { commissioningDirty.delete(segmentId); if (selected === segmentId) renderCommissioning(liveState.segments.find(s => s.id === segmentId)); }
        if (result) commissioningCorrections.delete(segmentId);
        if (clampedMaximum !== null && result) { const message = `Letzte LED auf ${clampedMaximum} begrenzt (Summe der LED-Anzahlen) · live angewendet.`; status(message, 'ok', 5000); el('commission-status').textContent = message; }
        return result;
    }
    for (const key of ['start','end']) el(key + '-led').addEventListener('input', event => {
        commissioningDrafts.get(selected)[key] = Number(event.target.value); commissioningDirty.add(selected);
        el('commission-status').textContent = UNSAVED;
        clampCommissioningEnd(); scheduleCommissioning();
    });
    for (const key of ['start','end']) el(key + '-led').addEventListener('change', () => { if (!autoApply) clampCommissioningEnd(); scheduleCommissioning(0); });
    el('commission-apply').addEventListener('click', () => stageCommissioning(true));
    root.querySelectorAll('[data-test]').forEach(button => button.addEventListener('click', () => sendCommand('testStart', {pattern:button.dataset.test,level:Number(el('test-level').value)})));
    el('test-stop').addEventListener('click', () => sendCommand('testStop', null));
    el('test-level').addEventListener('input', () => { el('test-level-value').textContent = levelPercent(el('test-level').value); });
    // A running test restarts with the new level and the same pattern.
    let levelWanted = null;
    async function applyTestLevel() {
        if (busy || levelWanted === null) return;
        const s = current(), level = levelWanted;
        levelWanted = null;
        if (s?.testing && s.testPattern) await sendCommand('testStart', {pattern:s.testPattern,level});
    }
    el('test-level').addEventListener('change', () => { levelWanted = Number(el('test-level').value); applyTestLevel(); });

    // Tabs (arrow keys, Home, End) and the phone colour switch.
    const tabs = ['control','scenes','commission'];
    function selectTab(name) {
        for (const tab of tabs) {
            el('panel-' + tab).hidden = tab !== name;
            el('tab-' + tab).setAttribute('aria-selected', String(tab === name));
            el('tab-' + tab).tabIndex = tab === name ? 0 : -1;
        }
    }
    tabs.forEach((tab, index) => {
        el('tab-' + tab).addEventListener('click', () => selectTab(tab));
        el('tab-' + tab).addEventListener('keydown', event => {
            if (!['ArrowLeft','ArrowRight','Home','End'].includes(event.key)) return;
            event.preventDefault(); const next = event.key === 'Home' ? 0 : event.key === 'End' ? tabs.length-1 : (index+(event.key==='ArrowRight'?1:-1)+tabs.length)%tabs.length;
            selectTab(tabs[next]); el('tab-' + tabs[next]).focus();
        });
    });
    root.querySelectorAll('[data-colour]').forEach(button => button.addEventListener('click', () => {
        root.dataset.activeColour = button.dataset.colour;
        root.querySelectorAll('[data-colour]').forEach(b => b.setAttribute('aria-pressed', String(b === button)));
    }));
    poll();
})();
