// neopixel.js - NeoPixel status page /neo: renders this device from /neo/state, /neo/em and /neo/pixels.
(function () {
  'use strict';
  var ROOT = document.getElementById('neo');
  if (!ROOT) return;
  ROOT.innerHTML = "<div class=\"np-head\">\n  <div><h1>NeoPixel</h1><div class=\"meta\" id=\"meta\"></div></div>\n</div>\n<div class=\"np-barw\"><div class=\"np-bar\" id=\"bar\"></div><div class=\"np-rf\"><button class=\"np-btn\" id=\"rf-now\" title=\"Jetzt aktualisieren\" aria-label=\"Jetzt aktualisieren\">↻</button><select class=\"np-sel\" id=\"rf-sel\" aria-label=\"Aktualisierung\"><option value=\"1\">1 s</option><option value=\"2\">2 s</option><option value=\"5\">5 s</option><option value=\"10\">10 s</option><option value=\"30\">30 s</option><option value=\"60\">60 s</option><option value=\"0\">aus</option></select></div></div>\n<div class=\"np-note bad\" id=\"unconf\" hidden></div>\n<div class=\"np-tabs\" id=\"tabs\">\n  <button class=\"np-tab active\" data-t=\"hw\">Hardware</button>\n  <button class=\"np-tab\" data-t=\"setup\">Aufbau</button>\n  <button class=\"np-tab\" data-t=\"ctl\">Steuern</button>\n  <button class=\"np-tab\" data-t=\"segs\">Segmente</button>\n  <button class=\"np-tab\" data-t=\"em\">Effektmanager</button>\n  <button class=\"np-tab\" data-t=\"chain\">Effektkette</button>\n  <button class=\"np-tab\" data-t=\"strips\">Streifen, Strom &amp; Relais</button>\n</div>\n\n<section class=\"np-pane\" id=\"p-hw\"><div id=\"hw\"></div></section>\n<section class=\"np-pane\" id=\"p-setup\" hidden>\n  <div class=\"np-card\">\n    <div class=\"np-card-h\"><h3>Anordnung \u00b7 Draufsicht mit LED-Bild</h3><span id=\"lay-name\" class=\"np-pill idle\"></span><span class=\"np-sp\"></span><button class=\"np-btn\" id=\"lay-edit\">Anordnung festlegen</button></div>\n    <canvas class=\"np-stage\" id=\"plan\"></canvas>\n    <div class=\"np-cap\" id=\"plan-cap\"></div>\n  </div>\n  <div class=\"np-editor\" id=\"editor\" hidden></div>\n  <div class=\"np-note info\"><b>Die Anordnung legst du fest.</b> Die Firmware wei\u00df nicht, wo die LEDs liegen: Sie kennt nur die Reihenfolge der Streifen, die Segmente, die 2D/3D-Geometrie mit Verdrahtung und den Versatz in der Effektkette. Die Anordnung ist eine Anzeigehilfe und \u00e4ndert nichts an der LED-Ausgabe. Sie wird in diesem Browser gespeichert und l\u00e4sst sich als JSON-Datei exportieren und in einem anderen Browser importieren; Speichern auf dem Ger\u00e4t (/neo/layout.json) ist eine sp\u00e4tere Option.</div>\n  <h2>Vom Streifen zum Segment</h2>\n  <div class=\"np-pipe\" id=\"pipe\"></div>\n  <div class=\"np-note\">LED-Bild = Inhalt des virtuellen Streifens, ohne HCL, Gamma und Strombegrenzung. Ausgeschaltete LEDs erscheinen hellgrau.</div>\n</section>\n<section class=\"np-pane\" id=\"p-ctl\" hidden><div id=\"ctl\"></div></section>\n<section class=\"np-pane\" id=\"p-segs\" hidden><div class=\"np-tw\" id=\"segs\"></div></section>\n<section class=\"np-pane\" id=\"p-em\" hidden><div id=\"em\"></div></section>\n<section class=\"np-pane\" id=\"p-chain\" hidden><div id=\"chain\"></div></section>\n<section class=\"np-pane\" id=\"p-strips\" hidden><div id=\"strips\"></div></section>\n";
  var $ = function (id) { return document.getElementById(id); };
  var qa = function (s, r) { return Array.prototype.slice.call((r || document).querySelectorAll(s)); };
  var cs0 = getComputedStyle(document.documentElement);
  function tok(n) { return cs0.getPropertyValue(n).trim(); }
  var TONE = [], EDGE = [];
  for (var k0 = 0; k0 < 8; k0++) { TONE.push(tok('--np-t' + k0)); EDGE.push(tok('--np-e' + k0)); }
  var GREEN = tok('--np-green'), GREENT = tok('--np-green-t'), INK = tok('--np-ink'), MUTE = tok('--np-mute'),
    FAINT = tok('--np-faint'), STAGE = tok('--np-stage'), LINE = tok('--np-line2'),
    BAD = tok('--np-bad'), BADBG = tok('--np-bad-bg'), BADFG = tok('--np-bad-fg'), OFFE = tok('--np-offedge');
  var OFF = rgbOf(tok('--np-off'));
  // White tones of the strip's white channels, mixed in additively: single W neutral, WW warm, CW cool.
  var TW = [255, 241, 220], TWW = [255, 215, 160], TCW = [242, 246, 255];
  function shown(r, g, b, w, cw, layout) { // the colour the strip emits, as RGB for the screen
    var a = layout === 'RGBCCT' ? TWW : TW, d = [r, g, b];
    if (layout !== 'RGB') { for (var i = 0; i < 3; i++) d[i] += (w || 0) * a[i] / 255 + (layout === 'RGBCCT' ? (cw || 0) * TCW[i] / 255 : 0); }
    return [Math.min(255, d[0]), Math.min(255, d[1]), Math.min(255, d[2])];
  }
  function pale(c) { return lum(c) > .82 && Math.max(c[0], c[1], c[2]) - Math.min(c[0], c[1], c[2]) < 80; } // white-ish: would vanish on the light stage
  function halo(col) { return pale(col) ? rgb(mix(col, [150, 140, 120], .45)) : rgb(col); } // light LEDs get a visible glow
  var SEGT = function (i) { return (i + 1) % 8; }, CUET = function (k) { return [2, 1, 3, 4, 5, 6, 0, 7][k % 8]; }, NODET = [0, 1, 3, 4];
  var TOPO = ['1D', 'Zeilen serpentin', 'Zeilen linear', 'Spalten serpentin', 'Spalten linear', 'Ebene für Ebene, Zeilen serpentin', '2D gekachelt, Spalten linear', '2D gekachelt, Spalten serpentin'];

  // ---- helpers ------------------------------------------------------------------------------------
  function rgbOf(h) { h = h.replace('#', ''); return [parseInt(h.substr(0, 2), 16), parseInt(h.substr(2, 2), 16), parseInt(h.substr(4, 2), 16)]; }
  function rgb(c) { return 'rgb(' + (c[0] | 0) + ',' + (c[1] | 0) + ',' + (c[2] | 0) + ')'; }
  function mix(a, b, k) { return [a[0] + (b[0] - a[0]) * k, a[1] + (b[1] - a[1]) * k, a[2] + (b[2] - a[2]) * k]; }
  function lum(c) { return (.3 * c[0] + .59 * c[1] + .11 * c[2]) / 255; }
  function clamp(v, a, b) { return v < a ? a : v > b ? b : v; }
  function fmt(n) { return String(Math.round(n)).replace(/\B(?=(\d{3})+(?!\d))/g, ' '); }
  function tfmt(s) { var d = Math.round(s * 10), m = Math.floor(d / 600), r = (d - m * 600) / 10; return m + ':' + (r < 10 ? '0' : '') + String(r).replace('.', ','); }
  function mmss(s) { s = Math.floor(s); return Math.floor(s / 60) + ':' + ('0' + s % 60).slice(-2); }
  function dur(d) { return d === 0 ? 'halten' : d === 0xFFFF ? 'bis Effekt fertig' : d >= 60 ? mmss(d) + ' min' : d + ' s'; }
  function pill(cls, txt) { return '<span class="np-pill ' + cls + '"><span class="np-dot"></span>' + txt + '</span>'; }
  function meter(pct, cls, mark) { return '<span class="np-meter ' + (cls || '') + '"><i style="width:' + clamp(pct, 0, 100) + '%"></i>' + (mark ? '<s style="left:' + mark + '%"></s>' : '') + '</span>'; }
  function badge(t, txt) { return '<span class="np-badge t' + t + '">' + txt + '</span>'; }
  function rr(c, x, y, w, h, r) { c.beginPath(); c.moveTo(x + r, y); c.arcTo(x + w, y, x + w, y + h, r); c.arcTo(x + w, y + h, x, y + h, r); c.arcTo(x, y + h, x, y, r); c.arcTo(x, y, x + w, y, r); c.closePath(); }
  function fit(c, s, w) { if (c.measureText(s).width <= w) return s; while (s.length > 1 && c.measureText(s + '…').width > w) s = s.slice(0, -1); return s + '…'; }
  function clone(o) { return JSON.parse(JSON.stringify(o)); }

  var tab = 'hw', sc, lay;
  var T0 = performance.now(), last = 0, power = null;

  // ---- live device ---------------------------------------------------------------------------------
  // /neo/state is built in the device's loop task about once per second; /neo/pixels is a copy of the
  // virtual strip (logical order, before HCL, gamma and current limiting); /neo/em holds the EM definitions.
  var LIVE = { st: null, ems: [], emSum: -1, emBusy: 0, px: null, rxAt: 0, ok: 0, err: '', sig: '', power: null, pend: {}, lastSeq: 0, tok: '', unconf: null };
  var LSN = ['ohne', 'TXS0108E', 'SN74HCT125', 'SN74AHCT125', 'SN74LV8T245'];
  var BUILDN = ['KNX TP', 'KNX TP + WLAN', 'KNX IP (WLAN)', 'KNX IP (LAN)'];
  var ERRN = { 1: 'Hardware passt nicht zur Gerätekonfiguration', 2: 'keine Hardware in der Gerätekonfiguration gewählt', 3: 'GPIO doppelt belegt', 4: 'Streifen nicht angelegt', 5: 'virtueller Streifen nicht angelegt' };
  var WARNN = { 2: 'gespeicherte Daten verworfen (Konfiguration geändert)', 3: 'unbekannter Effekt in der Gerätekonfiguration' };
  var RESN = ['Übernommen.', 'Gesperrt: der Befehl wird ignoriert.', 'Startverzögerung läuft: der Befehl wird zurückgestellt und danach abgearbeitet.', 'Ungültiger Befehl: ignoriert.', 'EM nicht startbar (nicht aktiv, ohne Cues oder nicht verfügbar): ignoriert.', 'Szene nicht angelegt: ignoriert.', 'Kein EM läuft: ignoriert.', 'Startverzögerung läuft: der Befehl wird verworfen (Dimmen und Objekte ohne Schreiben-Flag wirken erst danach).'];
  function esc(t) { return String(t).replace(/[&<>"']/g, function (c) { return { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]; }); }
  function hex6(n) { return '#' + ('00000' + (n >>> 0).toString(16)).slice(-6); }
  function hex4(n) { return '0x' + ('000' + (n >>> 0).toString(16).toUpperCase()).slice(-4); }
  function upStr(s) { var d = Math.floor(s / 86400), h = Math.floor(s % 86400 / 3600), m = Math.floor(s % 3600 / 60), x = s % 60; return d + ' d ' + ('0' + h).slice(-2) + ':' + ('0' + m).slice(-2) + (d || h ? '' : ':' + ('0' + x).slice(-2)); }
  function fxName(id) { var n = FX1D.concat(FX2D)[id]; return n || 'Effekt ' + id; }
  var EMPTY = { v: 1, up: 0, leds: 0, bpl: 3, bar: { pw: 1, bri: 0, fps: 0, fpsT: 0, hcl: 0, err: 0, warn: 0, warn2: 0 }, pwr: { mode: 0, lim: 0, thr: 0, abl: 0, perLed: 0, ma: 0, load: 0 },
    hw: { chip: '', mhz: 0, cores: 0, hwid: 0, board: '', ets: 0, etsName: '', ls: 0, build: 0, ft: 0, ftMin: 0, ftMax: 0, temp: 0, heap: 0, heapMin: 0, heapTot: 0, rmtFree: 0, rmtTot: 0, start: 1, startLeft: 0, deferred: 0, savedValid: 0 },
    emCount: 0, emSum: 0, strips: [], segs: [], relays: [], cmds: [], empty: 1 };
  // Converts the device snapshot into the page model.
  function fromState(j, ems) {
    var L = { emCount: j.emCount || 0 }, i, k;
    var layout = j.bpl === 5 ? 'RGBCCT' : j.bpl === 4 ? 'RGBW' : 'RGB', start = !!j.hw.start;
    L.bar = { power: j.bar.pw, bri: j.bar.bri, layout: layout, vleds: j.leds || 0, fps: j.bar.fps, fpsT: j.bar.fpsT, hcl: j.bar.hcl, notes: [] };
    L.pw = { mode: j.pwr.mode === 1 ? 'fix' : j.pwr.mode === 2 ? 'led' : 'off', lim: j.pwr.lim, thr: j.pwr.thr, abl: j.pwr.abl, perLed: j.pwr.perLed };
    L.strips = j.strips.filter(Boolean).map(function (s) {
      var pm = ['none', 'global', 'fix', 'led'][s.pm] || 'global', lim = s.sl || (pm === 'fix' ? s.lim : pm === 'led' ? s.n * s.perLed : 0);
      return { name: '', chip: esc(s.p), leds: s.n, port: (s.ch >= 0 ? 'Kanal ' + (s.ch + 1) + ' · ' : '') + 'GPIO ' + s.pin + (s.spi ? ' / CLK ' + s.clk : ''), order: esc(s.order), ch: j.bpl, drv: s.spi ? 'SPI' : 'RMT',
        tim: s.spi ? '<span class="np-f">— SPI</span>' : s.tim + ' kHz · ' + (s.tov ? 'Override' : 'Chipprofil'), pm: pm, lim: lim, perLed: s.perLed, thr: s.thr, abl: s.abl, spi: s.spi, vs: s.vs, ma: s.ma, load: s.load };
    });
    L.segs = j.segs.map(function (s, i) {
      var c = s.ch, scenes = [];
      for (k = 1; k <= s.sc; k++) scenes.push({ n: k });
      return { a: s.a, b: s.b, geo: is1D(s.geo) ? [0] : s.geo.slice(0, 4), st: !start ? 'wait' : s.lock && s.vis ? 'lock' : s.vis ? 'on' : 'off', locked: s.lock, bri: s.bri,
        c1: s.c, w: s.w, cw: layout === 'RGBCCT' ? s.cw : 0, c2: s.c2 ? hex6(s.c2) : '', fx: fxName(s.fx), fxId: s.fx,
        em: s.rs ? s.em : 0, cue: s.cue, ems: s.rs === 1 ? 'run' : s.rs === 2 ? (s.susp ? 'susp' : 'pause') : 'stop', cueEl: s.el / 1000, stopRet: s.ret === 2 ? 'Aus' : s.ret === 1 ? 'Default' : 'Letzter Zustand',
        ch: c.m ? { m: c.m, pol: c.pol, to: c.to, total: c.total, off: c.off, age: Math.max(0, c.m === 2 ? c.rx : c.tx), none: (c.m === 2 ? c.rx : c.tx) < 0, timeout: !!c.tmo, wait: !start, override: !!c.ov, pl: c.pl } : { m: 0 },
        hcl: s.hcl, hclSrc: s.hm === 1 ? 'Lichtmanager ' + s.hl + ' (global)' : s.hm === 2 ? 'eigener Lichtmanager-Kanal ' + s.hl : '', scenes: scenes.length ? scenes : null, scene: s.scene, gs: s.gs };
    });
    L.ems = (ems || []).filter(function (e) { return e.id <= L.emCount; }).map(function (e) {
      return { id: e.id, name: '', st: e.st, loop: e.loop, next: e.next, cues: e.cues.map(function (q) { return { n: esc(q.n) || '—', fx: fxName(q.fx), c: q.c, w: q.w, bri: q.bri === 100 ? 0 : q.bri, d: q.d, f: q.f }; }) };
    });
    L.relays = j.relays.map(function (r) { return { name: '', st: r.st, inv: r.inv, pin: 'GPIO ' + r.pin, nopin: r.pin === 255, lock: r.lock, offDelay: r.off, minOff: r.min, pend: r.pend, to: r.to }; });
    var w = j.hw, used = [];
    for (k = 0; k < (w.rmtTot || 0); k++) used.push(k < w.rmtTot - w.rmtFree ? 1 : 0);
    var nv = L.segs.length;
    L.hw = { chip: w.chip ? w.chip + ' · ' + w.mhz + ' MHz · ' + w.cores + (w.cores === 1 ? ' Kern' : ' Kerne') : '—', board: esc(w.board || 'unbekannt'), hwid: hex4(w.hwid), ls: LSN[w.ls] || String(w.ls), build: BUILDN[w.build] || 'KNX',
      ft: Math.round(w.ft / 100) / 10, ftMin: Math.round(w.ftMin / 100) / 10, ftMax: Math.round(w.ftMax / 100) / 10, heap: Math.round(w.heap / 1024), heapMin: Math.round(w.heapMin / 1024), heapTot: Math.round(w.heapTot / 1024) || 1,
      rmt: used, temp: w.temp / 10, up: upStr(j.up), start: start ? 'abgeschlossen' : w.startLeft > 0 ? 'Startverzögerung · noch ca. ' + Math.ceil(w.startLeft / 1000) + ' s · ' + w.deferred + ' Telegramme gepuffert' : 'Wiederherstellung läuft · ' + w.deferred + ' Telegramme gepuffert',
      saved: !nv ? 'keine Segmente' : (w.savedValid || 0) + ' / ' + nv + (nv > 1 ? ' Segmente' : ' Segment'),
      err: j.bar.err ? 'Fehler ' + j.bar.err + ': ' + (ERRN[j.bar.err] || '') : WARNN[j.bar.warn] || WARNN[j.bar.warn2] || '' };
    if (w.ets) L.hw.ets = w.ets === 255 ? 'eine Auswahl (keine Hardware gewählt)' : (w.etsName ? esc(w.etsName) + ' (' + hex4(w.ets) + ')' : 'HW-ID ' + hex4(w.ets));
    // Effektkette: this device knows its own part only; the rest of the band is shown as unknown
    L.chain = null;
    for (i = 0; i < L.segs.length && !L.chain; i++) {
      var s = L.segs[i], c = s.ch, len = s.b - s.a + 1;
      if (!c.m || !(c.total > 0)) continue;
      var nodes = [], role = c.m === 1 ? 'Master' : 'Slave', peer = c.m === 1 ? 'Slave' : 'Master oder Slave', off = Math.min(c.off, c.total), end = Math.min(c.total, off + len);
      if (off > 0) nodes.push({ n: 'Kette 0–' + (off - 1), role: peer, off: 0, len: off, unk: 1 });
      nodes.push({ n: 'Segment ' + (i + 1), role: role, me: 1, off: off, len: Math.max(1, end - off), timeout: c.timeout, wait: c.wait });
      if (end < c.total) nodes.push({ n: 'Kette ' + end + '–' + (c.total - 1), role: peer, off: end, len: c.total - end, unk: 1 });
      L.chain = { seg: i, total: c.total, nodes: nodes };
    }
    // notes in the status bar
    var n = L.bar.notes;
    if (!j.bar.pw) n.push('Gerät aus');
    if (j.empty) n.push('Verbinde …');
    if (L.hw.ets) n.push('Hardware ≠ Konfiguration');
    if (j.bar.err && j.bar.err !== 1 && j.bar.err !== 2) n.push('Fehler ' + j.bar.err);
    if (!start) n.push('Startphase');
    L.segs.forEach(function (s, i) { if (s.locked) n.push('Segment ' + (i + 1) + ' gesperrt'); if (s.ch.timeout) n.push('Kette: kein Sync'); });
    if (w.heapTot && w.heapMin < 32 * 1024) n.push('Wenig Speicher');
    if (WARNN[j.bar.warn]) n.push(j.bar.warn === 2 ? 'Daten verworfen' : 'unbekannter Effekt');
    // live current from the device
    var P = { strips: [], limited: false, active: j.bar.warn === 1, total: 0, gMa: j.pwr.ma, load: j.pwr.load };
    L.strips.forEach(function (s) { var ma = s.ma > 0 ? s.ma : 0; P.total += ma; P.strips.push({ ma: ma, lim: s.lim, load: s.load }); });
    if (L.pw.mode === 'off') P.gMa = P.total;
    L.power = P;
    // default arrangement: one part per segment (first segment 3D: cube, 2D: matrix)
    var s0 = L.segs[0], g0 = s0 && s0.geo;
    if (g0 && g0[0] && g0[2] > 1 && !is2D(g0)) L.layout = { tpl: 'cube', p: { n: clamp(g0[0], 2, 16), pitch: 5 }, runs: [{ name: 'Würfel', src: 'seg', seg: 0, rev: 0 }] };
    else if (g0 && is2D(g0) && L.segs.length === 1) L.layout = { tpl: 'matrix', p: { cols: clamp(g0[0], 1, 64), rows: clamp(g0[1], 1, 64), pitch: 10 }, runs: [{ name: 'Matrix', src: 'seg', seg: 0, rev: 0 }] };
    else L.layout = { tpl: 'line', p: { n: clamp(L.segs.length, 1, 8), dens: 60, gap: 40 }, runs: L.segs.slice(0, 8).map(function (x, i) { return { name: 'Segment ' + (i + 1), src: 'seg', seg: i, rev: 0 }; }) };
    return L;
  }
  // A live LED: [R,G,B shown at full intensity, intensity, r,g,b,w,cw as in the buffer] or null when dark.
  function livePx(v) {
    var P = LIVE.px; if (!P || v < 0 || v >= P.n) return null;
    var d = P.d, o = 8 + v * P.bpl, r = d[o], g = d[o + 1], b = d[o + 2], w = P.bpl > 3 ? d[o + 3] : 0, cw = P.bpl > 4 ? d[o + 4] : 0;
    if (!(r | g | b | w | cw)) return null;
    var s = shown(r, g, b, w, cw, sc.bar.layout), m = Math.max(s[0], s[1], s[2]) || 1;
    // The buffer holds gamma-corrected, dimmed values (a lit LED is often 10-80 of 255): show it brighter
    // than linear - square root undoes gamma 2.0 - with a floor, so every lit LED stays clearly visible.
    var k = Math.min(1, m / 255);
    return [s[0] * 255 / m, s[1] * 255 / m, s[2] * 255 / m, .35 + .65 * Math.sqrt(k), r, g, b, w, cw];
  }

  // ---- pixel lookup -------------------------------------------------------------------------------
  function is1D(g) { return !(g[3] > 0) || !(g[0] > 1) || !(g[1] > 1); } // as LedGeometry::is1D: topology 0 whatever the dimensions
  function is2D(g) { return !is1D(g) && (!(g[2] > 1) || g[3] >= 6); } // as LedGeometry::is2D
  function fwd(tp, x, y, w, h) { // (x, y) -> local index for the firmware topologies; tiled is drawn like its column variant
    if (tp === 1 || tp === 2 || tp === 5) return y * w + (tp === 1 && (y & 1) ? w - 1 - x : x);
    return x * h + ((tp === 3 || tp === 7) && (x & 1) ? h - 1 - y : y);
  }
  // Segment, strip, EM and relay names exist only in the configuration tool (no Memory element), so the device cannot show them.
  // The page labels a segment or strip with the name of the arrangement part mapped to it (browser), else by number.
  function partName(src, idx) { var r = lay && lay.runs.filter(function (x) { return x.src === src && x[src === 'seg' ? 'seg' : 'strip'] === idx && x.on !== 0; })[0]; return r && r.name ? r.name : ''; }
  function segLabel(i) { var n = partName('seg', i); return n ? esc(n) : 'Segment ' + (i + 1); } // HTML: names come from imported files
  function stripLabel(i) { var n = partName('strip', i); return n ? esc(n) : 'Streifen ' + (i + 1); }
  // Segment table: next to the badge only an own name adds anything; the default part name "Segment n" repeats it.
  function segName(i) { var n = partName('seg', i); return n && n !== 'Segment ' + (i + 1) ? esc(n) : ''; }
  function emOf(id) { return sc.ems.filter(function (e) { return e.id === id; })[0]; }
  function emOn(s) { return !!s.em && /run|pause/.test(s.ems || ''); }
  // What a segment shows now: the active cue while an EM runs, else the direct state.
  function lookOf(s) { return { fx: s.fx, c1: s.c1, w: s.w, cw: s.cw, bri: s.bri }; } // the device reports the cue state as the segment state
  function bandSuspended(s) { return !!(s.ch.m && emOn(s)); } // EffektManager pauses the virtual band while an EM runs
  // li = LED index inside the panel, po = where that panel starts in the chain band (own offset for this device).
  // Returns [R,G,B shown (white mixed in), intensity, r,g,b,w,cw as in the buffer] or null; other chain devices are not visible from here.
  function segPx(s, li, t, po) { return po === own(s) && li <= s.b - s.a ? livePx(s.a + li) : null; }
  function own(s) { return s.ch.m ? s.ch.off : 0; }
  function virtLeds() { return sc.bar.vleds || sc.strips.reduce(function (a, s) { return a + s.leds; }, 0); } // virtual length, gaps included
  function stripStart(i) { if (sc.strips[i] && sc.strips[i].vs >= 0) return sc.strips[i].vs; var a = 0; for (var j = 0; j < i; j++) a += sc.strips[j].leds; return a; }
  function segAt(v) { for (var j = 0; j < sc.segs.length; j++) { var s = sc.segs[j]; if (v >= s.a && v <= s.b) return s; } return null; }
  function runLen(run) {
    if (run.src === 'seg') { var s = sc.segs[run.seg]; return s ? s.b - s.a + 1 : 0; }
    if (run.src === 'strip') { var st = sc.strips[run.strip]; return st ? st.leds : 0; }
    if (run.src === 'chain') { var nd = sc.chain && sc.chain.nodes.filter(function (n) { return n.off === run.base; })[0]; return nd ? nd.len : 0; }
    return 0;
  }
  function runPx(run, i, t) {
    var s;
    if (run.src === 'seg') {
      s = sc.segs[run.seg];
      if (!s || i > s.b - s.a) return null;
      return segPx(s, i, t, own(s));
    }
    if (run.src === 'strip') {
      var st = sc.strips[run.strip]; if (!st || i >= st.leds) return null;
      var v = stripStart(run.strip) + i; s = segAt(v);
      return s ? segPx(s, v - s.a, t, own(s)) : null;
    }
    return null; // chain: another device, its LEDs are not visible from here
  }
  function nodeOf(run) {
    var ch = sc.chain; if (!ch) return null;
    if (run.src === 'seg' && run.seg === ch.seg) return ch.nodes.filter(function (n) { return n.me; })[0];
    if (run.src === 'chain') return ch.nodes.filter(function (n) { return n.off === run.base; })[0] || null;
    return null;
  }
  function runTone(run) {
    var nd = nodeOf(run);
    if (nd) return NODET[sc.chain.nodes.indexOf(nd) % 4];
    if (run.src === 'seg') return SEGT(run.seg);
    if (run.src === 'strip') return run.strip % 8;
    return 7;
  }

  function stripLimit(st) { return st.pm === 'fix' ? st.lim : st.pm === 'led' ? st.leds * st.perLed : 0; }

  // ---- arrangement templates ----------------------------------------------------------------------
  var TPL = {
    rows: {
      name: 'Balkenreihen', icon: '<path d="M4 6h12M18 6h12M32 6h12M4 14h12M18 14h12M32 14h12M4 22h12M18 22h12M32 22h12"/>',
      prm: [['per', 'Balken je Abschnitt', 1, 16], ['len', 'Balkenlänge (cm)', 20, 600], ['gap', 'Balkenabstand (cm)', 5, 200], ['ggap', 'Fuge zwischen Abschnitten (cm)', 0, 400], ['leds', 'LEDs je Balken', 1, 300]],
      runs: function (p, r) { return Math.max(1, r || 3); }, part: 'Abschnitt', free: 1,
      build: function (p, runs) { // sections end to end in bar direction, bar b of every section on the same line
        var G = { lines: [], areas: [] };
        runs.forEach(function (r, g) {
          var x0 = g * (p.len + p.ggap);
          G.areas.push({ x: x0 - 6, y: -12, w: p.len + 12, h: (p.per - 1) * p.gap + 24, run: r });
          for (var b = 0; b < p.per; b++) G.lines.push({ run: r, x1: x0, y1: b * p.gap, x2: x0 + p.len, y2: b * p.gap, n: p.leds, i0: b * p.leds });
        });
        return G;
      }
    },
    bars: {
      name: 'Balkengruppen', icon: '<path d="M8 4v20M15 4v20M22 4v20M30 4v20M37 4v20"/>',
      prm: [['groups', 'Gruppen', 1, 6], ['per', 'Balken je Gruppe', 1, 16], ['len', 'Balkenlänge (cm)', 20, 600], ['gap', 'Balkenabstand (cm)', 5, 200], ['ggap', 'Gruppenabstand (cm)', 0, 400], ['leds', 'LEDs je Balken', 1, 300]],
      runs: function (p) { return p.groups; }, part: 'Gruppe',
      build: function (p, runs) {
        var gw = (p.per - 1) * p.gap, G = { lines: [], areas: [] };
        runs.forEach(function (r, g) {
          var x0 = g * (gw + p.ggap);
          G.areas.push({ x: x0 - 14, y: -12, w: gw + 28, h: p.len + 24, run: r });
          for (var b = 0; b < p.per; b++) G.lines.push({ run: r, x1: x0 + b * p.gap, y1: 0, x2: x0 + b * p.gap, y2: p.len, n: p.leds, i0: b * p.leds });
        });
        return G;
      }
    },
    u: {
      name: 'Garage', icon: '<path d="M10 6h30v16H10M10 6v16" stroke-dasharray="30 0"/>', walls: 1,
      prm: [['w', 'Innenbreite (cm)', 100, 1000], ['d', 'Tiefe (cm)', 100, 1500]],
      runs: function () { return 4; }, names: ['Einfahrt', 'Wand links', 'Rückwand', 'Wand rechts'],
      build: function (p, runs) { // four walls in drive-in order; each can be switched off, LED count from its source
        var x0 = 520, G = { lines: [], areas: [{ x: 0, y: -30, w: x0, h: p.w + 60, drive: 1 }, { x: x0, y: 0, w: p.d, h: p.w, garage: 1, back: runs[2] && runs[2].on !== 0 }] };
        // [x1, y1, x2, y2, label suffix, label x, label y, pixel offset (null = above the door), align]
        var geo = [[x0, 12, x0, p.w - 12, '', x0, 0, null, 'center'],
          [x0 + 14, 14, x0 + p.d - 14, 14, [' · LED 0 vorn →', ' · LED 0 hinten ←'], x0 + 14, 14, 16, 'left'],
          [x0 + p.d - 14, 14, x0 + p.d - 14, p.w - 14, [' · LED 0 bei Wand links', ' · LED 0 bei Wand rechts'], x0 + p.d - 28, p.w / 2, 0, 'rot'],
          [x0 + 14, p.w - 14, x0 + p.d - 14, p.w - 14, [' · LED 0 vorn →', ' · LED 0 hinten ←'], x0 + 14, p.w - 14, -15, 'left']];
        runs.forEach(function (r, i) {
          if (!r || r.on === 0) return;
          var g = geo[i], n = runLen(r), lab = i === 0 ? r.name + (n ? ' (über dem Tor)' : ' · nicht belegt') : r.name + (n ? g[4][r.rev ? 1 : 0] : ' · nicht belegt');
          G.lines.push({ run: r, x1: g[0], y1: g[1], x2: g[2], y2: g[3], n: n, i0: 0, empty: !n, lab: { t: lab, x: g[5], y: g[6], off: g[7], al: g[8] } });
        });
        return G;
      }
    },
    line: {
      name: 'Linien', icon: '<path d="M4 14h12M20 14h12M36 14h8"/>',
      prm: [['n', 'Teile', 1, 8], ['dens', 'LEDs pro Meter', 10, 144], ['gap', 'Abstand (cm)', 0, 500]],
      runs: function (p) { return p.n; }, part: 'Teil', wrap: 1,
      build: function (p, runs, cols) { // part length from the LED count of its source; cols wraps the parts into rows (wrapRows sets y)
        var G = { lines: [], areas: [] }, x = 0;
        runs.forEach(function (r, i) {
          var n = runLen(r) || 60, len = n / p.dens * 100, row = cols ? Math.floor(i / cols) : 0;
          if (cols && i % cols === 0) x = 0;
          G.areas.push({ x: x - 10, y: -16, w: len + 20, h: 32, run: r, row: row }); G.lines.push({ run: r, x1: x, y1: 0, x2: x + len, y2: 0, n: n, i0: 0 });
          x += len + Math.max(p.gap, 30);
        });
        return G;
      }
    },
    matrix: {
      name: 'Rechteck-Matrix', icon: '<path d="M8 6h32M8 12h32M8 18h32M8 24h32" stroke-dasharray="2 3"/>',
      prm: [['cols', 'Spalten', 1, 64], ['rows', 'Zeilen', 1, 64], ['pitch', 'Raster (mm)', 5, 200]],
      runs: function () { return 1; }, names: ['Matrix'],
      build: function (p, runs) { // wiring follows the segment topology when the source is a 2D segment
        var q = p.pitch / 10, r = runs[0], s = r.src === 'seg' && sc.segs[r.seg], tp = s && is2D(s.geo) ? s.geo[3] : 2;
        var G = { lines: [], areas: [{ x: -q, y: -q, w: (p.cols + 1) * q, h: (p.rows + 1) * q, run: r }] };
        for (var y = 0; y < p.rows; y++) (function (y) { G.lines.push({ run: r, x1: -q / 2, y1: y * q, x2: (p.cols - .5) * q, y2: y * q, n: p.cols, i0: 0, map: function (k) { return fwd(tp, k, y, p.cols, p.rows); } }); })(y);
        return G;
      }
    },
    cube: {
      name: 'Würfel', icon: '<path d="M12 9h18v15H12zM12 9l7-5h18l-7 5M30 24l7-5V4"/>', cube: 1,
      prm: [['n', 'LEDs je Kante', 2, 16], ['pitch', 'Raster (cm)', 1, 30]],
      runs: function () { return 1; }, names: ['Würfel']
    }
  };
  var DEF = { rows: { per: 8, len: 100, gap: 30, ggap: 10, leds: 60 }, bars: { groups: 1, per: 8, len: 100, gap: 30, ggap: 80, leds: 60 }, u: { w: 300, d: 600 }, line: { n: 2, dens: 60, gap: 40 }, matrix: { cols: 16, rows: 16, pitch: 10 }, cube: { n: 8, pitch: 5 } };
  function syncRuns() {
    var T = TPL[lay.tpl], n = T.free ? T.runs(lay.p, lay.runs.length) : T.runs(lay.p);
    while (lay.runs.length < n) { var i = lay.runs.length; lay.runs.push({ name: T.names ? T.names[i] : T.part + ' ' + (i + 1), src: 'none', rev: 0, on: 1 }); }
    lay.runs.length = n;
    if (T.walls) lay.runs.forEach(function (r) { if (r.on === undefined) r.on = 1; });
  }

  // ---- arrangement plan (canvas) ----------------------------------------------------------------
  var plan = { G: null, cube: { yaw: .6, pitch: .45, zoom: 1, auto: true } };
  function bounds(G) {
    var b = [1e9, 1e9, -1e9, -1e9];
    G.areas.forEach(function (a) { b[0] = Math.min(b[0], a.x); b[1] = Math.min(b[1], a.y); b[2] = Math.max(b[2], a.x + a.w); b[3] = Math.max(b[3], a.y + a.h); });
    G.lines.forEach(function (l) { b[0] = Math.min(b[0], l.x1, l.x2); b[1] = Math.min(b[1], l.y1, l.y2); b[2] = Math.max(b[2], l.x1, l.x2); b[3] = Math.max(b[3], l.y1, l.y2); });
    if (!(b[2] >= b[0] && b[3] >= b[1])) return { x: 0, y: 0, w: 100, h: 10 }; // nothing to place (no segment): small empty stage
    return { x: b[0], y: b[1], w: b[2] - b[0], h: b[3] - b[1] };
  }
  function sizeCanvas(cv, h) {
    var w = cv.clientWidth || cv.parentNode.clientWidth || 800, dpr = window.devicePixelRatio || 1;
    cv.style.height = h + 'px'; cv.width = Math.round(w * dpr); cv.height = Math.round(h * dpr);
    return { w: w, h: h, dpr: dpr };
  }
  // The top view and the editor's LED image share one renderer; plan points at the one being laid out or drawn.
  var planMain = plan, planEd = {};
  function onPlan(P, fn) { var s0 = plan; P.cube = planMain.cube; plan = P; try { fn(); } finally { plan = s0; } }
  function edOpen() { return !$('editor').hidden && $('edplan'); }
  function layoutPlan() {
    onPlan(planMain, function () { layoutPlan0($('plan'), 380); });
    if (edOpen()) onPlan(planEd, function () { layoutPlan0($('edplan'), 230); });
  }
  function drawPlan(t) {
    onPlan(planMain, function () { drawPlan0(t); });
    if (edOpen() && planEd.sz) onPlan(planEd, function () { drawPlan0(t); });
  }
  function areaLabel(a) { return a.run.name + (a.run.src === 'none' ? ' · nicht belegt' : ''); }
  // Linien: as many parts per row as leave every name room up to the next part; rows share one scale.
  function wrapRows(T, cv, w, maxH, pad) {
    var c = cv.getContext('2d'), n = lay.runs.length, G, b, s, ox, k, y = 0;
    c.font = '600 11px sans-serif';
    for (k = n; k >= 1; k--) {
      G = T.build(lay.p, lay.runs, k); G.rows = Math.ceil(n / k); b = bounds(G); s = Math.min((w - 2 * pad) / Math.max(1, b.w), maxH / (32 * G.rows)); ox = pad + ((w - 2 * pad) - b.w * s) / 2 - b.x * s;
      if (G.areas.every(function (a, i) { if (nodeOf(a.run)) return true; var nx = G.areas[i + 1], room = nx && nx.row === a.row ? (nx.x - a.x) * s : w - 4 - ox - a.x * s; return c.measureText(areaLabel(a)).width + 8 <= room; })) break;
    }
    if (!k) { G = T.build(lay.p, lay.runs, n); G.rows = 1; } // nothing fits: one row, names cut as before
    G.areas.forEach(function (a, i) { // row gap in px: label line, or bus and pill of a chained part
      if (i && a.row !== G.areas[i - 1].row) y += 32 + (G.areas.some(function (q) { return q.row === a.row && nodeOf(q.run); }) ? 76 : 30) / s;
      a.y += y; G.lines[i].y1 = G.lines[i].y2 = y;
    });
    return G;
  }
  function layoutPlan0(cv, maxH) {
    var T = TPL[lay.tpl], w = cv.clientWidth || 800;
    plan.cv = cv; plan.G = null; plan.chained = false;
    if (cv.id === 'plan') { cv.style.cursor = T.cube ? 'grab' : 'default'; cv.style.touchAction = T.cube ? 'none' : 'auto'; }
    if (T.cube) { plan.sz = sizeCanvas(cv, Math.round(Math.min(maxH + 40, clamp(w * .5, 300, 420)))); return; }
    var pad = 22, G = T.wrap ? wrapRows(T, cv, w, maxH, pad) : T.build(lay.p, lay.runs), b;
    G.lines.forEach(function (L) { L.run.N = 0; });
    G.lines.forEach(function (L) { L.run.N = Math.max(L.run.N, L.map ? L.n * lay.p.rows : L.i0 + L.n); });
    G.lines.forEach(function (L) { L.runN = L.run.N; });
    b = bounds(G); var chained = G.areas.some(function (a) { return a.run && nodeOf(a.run); }), chained0 = G.areas.some(function (a) { return !a.row && a.run && nodeOf(a.run); });
    var top = chained0 ? (lay.edge ? 96 : 76) : (lay.edge ? 36 : 30), bot = lay.edge ? 34 : 22;
    var s = Math.min((w - 2 * pad) / Math.max(1, b.w), maxH / Math.max(1, T.wrap ? 32 * G.rows : b.h)); // wrapped: all rows together share maxH
    plan.s = s; plan.ox = pad + ((w - 2 * pad) - b.w * s) / 2 - b.x * s; plan.oy = top - b.y * s; plan.b = b; plan.top = top; plan.G = G; plan.chained = chained;
    plan.sz = sizeCanvas(cv, Math.round(b.h * s + top + bot));
  }
  // One LED line: light track, pale-grey dark LEDs, lit LEDs in their colour with a soft halo.
  function drawLine(c, L, X, Y, t) {
    if (!(L.n >= 1)) return;
    var x1 = X(L.x1), y1 = Y(L.y1), x2 = X(L.x2), y2 = Y(L.y2), n = L.n, len = Math.hypot(x2 - x1, y2 - y1), pp = len / n, vert = Math.abs(x2 - x1) < Math.abs(y2 - y1), dots = pp >= 5;
    var cols = [], k, f, px, py, col, a, m, idx;
    for (k = 0; k < n; k++) { idx = L.map ? L.map(k) : L.i0 + k; cols.push(runPx(L.run, L.run.rev ? L.runN - 1 - idx : idx, t)); } // "LED 0 am Ende" reverses the whole part
    if (!dots) { c.fillStyle = OFFE; if (vert) c.fillRect(x1 - 3.5, Math.min(y1, y2) - 1, 7, Math.abs(y2 - y1) + 2); else c.fillRect(Math.min(x1, x2) - 1, y1 - 3.5, Math.abs(x2 - x1) + 2, 7); }
    for (k = 0; k < n; k++) { // halos first, so neighbouring cores stay crisp
      col = cols[k]; if (!col || col[3] < .05) continue;
      f = (k + .5) / n; px = x1 + (x2 - x1) * f; py = y1 + (y2 - y1) * f; a = col[3];
      c.globalAlpha = .22 * a; c.fillStyle = halo(col);
      if (dots) { c.beginPath(); c.arc(px, py, Math.min(pp * .9, 9), 0, 6.283); c.fill(); }
      else if (vert) c.fillRect(px - 7, py - pp / 2, 14, pp + .5); else c.fillRect(px - pp / 2, py - 7, pp + .5, 14);
    }
    c.globalAlpha = 1;
    for (k = 0; k < n; k++) {
      col = cols[k]; f = (k + .5) / n; px = x1 + (x2 - x1) * f; py = y1 + (y2 - y1) * f;
      m = col ? mix(OFF, col, clamp(col[3], 0, 1)) : OFF;
      c.fillStyle = rgb(m);
      if (dots) { c.beginPath(); c.arc(px, py, Math.min(pp * .32, 3.2), 0, 6.283); c.fill(); c.strokeStyle = !col || col[3] < .05 ? OFFE : pale(m) ? 'rgba(150,115,60,.65)' : 'rgba(0,0,0,.12)'; c.lineWidth = 1; c.stroke(); }
      else if (vert) c.fillRect(px - 2.5, py - pp / 2, 5, pp + .6); else c.fillRect(px - pp / 2, py - 2.5, pp + .6, 5);
    }
    if (!dots) { // near-white light would vanish on the pale stage: outline the lit stretches
      c.strokeStyle = 'rgba(150,115,60,.65)'; c.lineWidth = 1;
      for (k = 0; k < n; k++) {
        col = cols[k]; if (!col || col[3] < .3 || !pale(col)) continue;
        var k2 = k; while (k2 + 1 < n && cols[k2 + 1] && cols[k2 + 1][3] >= .3 && pale(cols[k2 + 1])) k2++;
        var fa = k / n, fb = (k2 + 1) / n, ax = x1 + (x2 - x1) * fa, ay = y1 + (y2 - y1) * fa, bx = x1 + (x2 - x1) * fb, by = y1 + (y2 - y1) * fb;
        if (vert) c.strokeRect(ax - 3, Math.min(ay, by), 6, Math.abs(by - ay)); else c.strokeRect(Math.min(ax, bx), ay - 3, Math.abs(bx - ax), 6);
        k = k2;
      }
    }
  }
  // KNX group telegrams from the master to all slaves; marks the slaves a telegram just reached (p.arr).
  // A point may carry its own bus height (p.yb, one per row of the top view); rows are joined by a trunk at the left edge.
  function pillsAndBus(c, pts, yBus, t) {
    pts.forEach(function (p) { if (p.yb === undefined) p.yb = yBus; p.arr = 0; });
    var master = pts.filter(function (p) { return p.node.role === 'Master'; })[0], TX = 8;
    var ys = pts.map(function (p) { return p.yb; }).filter(function (y, i, a) { return a.indexOf(y) === i; }).sort(function (a, b) { return a - b; });
    function xsAt(y) { return pts.filter(function (p) { return p.yb === y; }).map(function (p) { return p.x; }); }
    c.strokeStyle = LINE; c.lineWidth = 2; c.setLineDash([]);
    ys.forEach(function (y) { var xs = xsAt(y); c.beginPath(); c.moveTo(ys.length > 1 ? TX : Math.min.apply(null, xs), y); c.lineTo(Math.max.apply(null, xs), y); c.stroke(); });
    if (ys.length > 1) { c.beginPath(); c.moveTo(TX, ys[0]); c.lineTo(TX, ys[ys.length - 1]); c.stroke(); }
    pts.forEach(function (p) {
      var broken = p.node.timeout, unk = p.node.unk && !broken;
      c.strokeStyle = broken ? BAD : unk ? LINE : FAINT; c.setLineDash(broken || unk ? [4, 4] : []);
      c.beginPath(); c.moveTo(p.x, p.yb); c.lineTo(p.x, p.y); c.stroke(); c.setLineDash([]);
      if (broken) { var my = (p.yb + p.y) / 2; c.strokeStyle = BAD; c.lineWidth = 2; c.beginPath(); c.moveTo(p.x - 5, my - 5); c.lineTo(p.x + 5, my + 5); c.moveTo(p.x + 5, my - 5); c.lineTo(p.x - 5, my + 5); c.stroke(); }
    });
    c.font = '11px sans-serif'; c.fillStyle = FAINT; c.textAlign = 'left'; c.textBaseline = 'bottom';
    var x0 = xsAt(ys[0]);
    if (ys.length > 1) c.fillText(fit(c, 'KNX · Sync-KO (Gruppenadresse)', Math.max(60, Math.max.apply(null, x0) - TX - 20)), TX + 6, ys[0] - 3);
    else if (x0.length > 1) c.fillText(fit(c, 'KNX · Sync-KO (Gruppenadresse)', Math.max(60, Math.abs(x0[1] - x0[0]) - 20)), Math.min.apply(null, x0) + 6, ys[0] - 3);
    function dot(a, b, f) { // position f (0..1) on the way pill a -> bus -> pill b
      var P = a.yb === b.yb ? [[a.x, a.y], [a.x, a.yb], [b.x, b.yb], [b.x, b.y]] : [[a.x, a.y], [a.x, a.yb], [TX, a.yb], [TX, b.yb], [b.x, b.yb], [b.x, b.y]], L = 0, i, l, d;
      for (i = 1; i < P.length; i++) L += Math.abs(P[i][0] - P[i - 1][0]) + Math.abs(P[i][1] - P[i - 1][1]);
      for (d = f * L, i = 1; i < P.length; i++) {
        l = Math.abs(P[i][0] - P[i - 1][0]) + Math.abs(P[i][1] - P[i - 1][1]);
        if (d <= l || i === P.length - 1) { var g = l ? Math.min(1, d / l) : 0, x = P[i - 1][0] + (P[i][0] - P[i - 1][0]) * g, y = P[i - 1][1] + (P[i][1] - P[i - 1][1]) * g; break; }
        d -= l;
      }
      c.fillStyle = GREEN; c.globalAlpha = .22; c.beginPath(); c.arc(x, y, 9, 0, 6.283); c.fill(); c.globalAlpha = 1; c.beginPath(); c.arc(x, y, 4, 0, 6.283); c.fill();
    }
    var ph = (t % 3) / 1.1; // heartbeat every 3 s; a change is sent at once (min. 300 ms apart)
    if (master && !master.node.unk) {
      pts.forEach(function (p) {
        if (p === master) return;
        if (ph > 1) { if (ph < 1.35) p.arr = 1; return; }
        dot(master, p, clamp(ph, 0, 1));
      });
    } else {
      // This device is a receiving slave and the master is not known by name: the telegram still
      // arrives here, so it runs from the other part of the chain to this device.
      var me = pts.filter(function (p) { return p.node.me; })[0], src = pts.filter(function (p) { return !p.node.me; })[0], cs0 = sc.segs[sc.chain.seg];
      if (me && src && me.node.role !== 'Master' && !me.node.timeout && !me.node.wait && cs0 && !cs0.ch.none) {
        if (ph > 1) { if (ph < 1.35) me.arr = 1; }
        else dot(src, me, clamp(ph, 0, 1));
      }
    }
  }
  function nodeBox(c, p, x, y, w, h, lines, arrived) {
    var n = p.node;
    rr(c, x, y, w, h, 7);
    c.fillStyle = n.timeout ? BADBG : n.me ? GREENT : '#fff'; c.fill();
    if (arrived && !n.me) { c.globalAlpha = .18; c.lineWidth = 6; c.strokeStyle = GREEN; c.stroke(); c.globalAlpha = 1; }
    c.lineWidth = n.me || n.timeout ? 2 : 1.2; c.strokeStyle = n.timeout ? BAD : n.me ? GREEN : FAINT;
    c.setLineDash(n.me || n.timeout ? [] : [4, 3]); c.stroke(); c.setLineDash([]);
    c.textAlign = 'center'; c.textBaseline = 'middle';
    lines.forEach(function (l, i) { c.font = (i === 0 ? '600 12px' : '11px') + ' sans-serif'; c.fillStyle = i === 0 ? INK : (n.timeout && i === lines.length - 1 ? BADFG : MUTE); c.fillText(fit(c, l, w - 10), x + w / 2, y + 12 + i * 15); });
  }
  function drawPlan0(t) {
    var cv = plan.cv, c = cv.getContext('2d'), z = plan.sz, G = plan.G;
    c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = STAGE; c.fillRect(0, 0, z.w, z.h);
    if (!G) return drawCube(c, z, t);
    var s = plan.s, X = function (x) { return plan.ox + x * s; }, Y = function (y) { return plan.oy + y * s; };
    function labRoom(a) { // Linien: a name may run up to the next part of its row (wrapRows reserved that room)
      if (!TPL[lay.tpl].wrap) return a.w * s - 4;
      var nx = G.areas[G.areas.indexOf(a) + 1]; return nx && nx.row === a.row ? (nx.x - a.x) * s - 8 : z.w - 8 - X(a.x);
    }
    c.textBaseline = 'middle';
    G.areas.forEach(function (a) {
      if (a.drive) { c.fillStyle = '#f1f2ee'; c.fillRect(X(a.x), Y(a.y), a.w * s, a.h * s); c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'left'; c.fillText('Zufahrt', X(a.x) + 8, Y(a.y) + 12); return; }
      if (a.garage) {
        c.fillStyle = '#f6f7f4'; c.fillRect(X(a.x), Y(a.y), a.w * s, a.h * s);
        var wt = Math.max(5, 18 * s);
        c.strokeStyle = '#e0e3dd'; c.lineWidth = wt; c.beginPath(); c.moveTo(X(a.x), Y(a.y) - wt / 2); c.lineTo(X(a.x + a.w) + wt / 2, Y(a.y) - wt / 2); c.lineTo(X(a.x + a.w) + wt / 2, Y(a.y + a.h) + wt / 2); c.lineTo(X(a.x), Y(a.y + a.h) + wt / 2); c.stroke();
        c.fillStyle = '#d3d7d0'; c.fillRect(X(a.x) - 3, Y(a.y) - wt, 6, wt); c.fillRect(X(a.x) - 3, Y(a.y + a.h), 6, wt);
        if (!a.back) { c.fillStyle = FAINT; c.font = '11px sans-serif'; c.textAlign = 'right'; c.fillText('Rückwand', X(a.x + a.w) - 10, Y(a.y + a.h / 2)); } return;
      }
      var nd = nodeOf(a.run), tn = runTone(a.run);
      rr(c, X(a.x), Y(a.y), a.w * s, a.h * s, 8); c.globalAlpha = .5; c.fillStyle = TONE[tn]; c.fill(); c.globalAlpha = 1;
      c.lineWidth = nd && nd.me ? 2 : 1; c.strokeStyle = nd && nd.me ? GREEN : nd && nd.timeout ? BAD : EDGE[tn]; c.setLineDash(nd && !nd.me ? [5, 4] : []); c.stroke(); c.setLineDash([]);
      if (nd && a.run.src !== 'none' && bandSuspended(sc.segs[sc.chain.seg]) && nd.me) { // EM: every device on its own panel
        var g0 = sc.segs[sc.chain.seg].geo; c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center';
        c.fillText(fit(c, 'EM ' + sc.segs[sc.chain.seg].em + ' · eigene ' + (is2D(g0) ? 'Matrix ' + g0[0] + ' × ' + g0[1] : 'LEDs'), a.w * s - 8), X(a.x + a.w / 2), Y(a.y) + 8);
      }
      if (!nd) { c.fillStyle = INK; c.font = '600 11px sans-serif'; c.textAlign = 'left'; c.fillText(fit(c, areaLabel(a), Math.max(30, labRoom(a))), X(a.x) + 4, Y(a.y) - 9); }
    });
    if (lay.edge) { c.fillStyle = FAINT; c.font = '11px sans-serif'; c.textAlign = 'center'; c.fillText('↑ ' + lay.edge[0], X(plan.b.x + plan.b.w / 2), plan.top - 14); c.fillText('↓ ' + lay.edge[1], X(plan.b.x + plan.b.w / 2), Y(plan.b.y + plan.b.h) + 18); }
    G.lines.forEach(function (L) {
      if (L.empty) { c.strokeStyle = LINE; c.lineWidth = 2; c.setLineDash([5, 4]); c.beginPath(); c.moveTo(X(L.x1), Y(L.y1)); c.lineTo(X(L.x2), Y(L.y2)); c.stroke(); c.setLineDash([]); }
      else drawLine(c, L, X, Y, t);
      if (L.lab) { // part labels inside the garage
        c.font = '11px sans-serif'; c.fillStyle = MUTE; c.textAlign = L.lab.al === 'rot' ? 'center' : L.lab.al; c.textBaseline = 'middle';
        var ly = L.lab.off === null ? Y(0) - Math.max(5, 18 * s) - 8 : Y(L.lab.y) + L.lab.off;
        if (L.lab.al === 'rot') { c.save(); c.translate(X(L.lab.x), ly); c.rotate(-Math.PI / 2); c.textAlign = 'center'; c.fillText(fit(c, L.lab.t, Math.max(40, (lay.p.w - 40) * s)), 0, 0); c.restore(); }
        else c.fillText(fit(c, L.lab.t, Math.max(40, L.lab.al === 'center' ? 300 : (lay.p.d - 40) * s)), X(L.lab.x), ly);
      }
    });
    if (plan.chained) {
      var pts = G.areas.filter(function (a) { return a.run && nodeOf(a.run); }).map(function (a) { var o = a.row ? Y(a.y) - 76 : 0; return { x: X(a.x + a.w / 2), y: 38 + o, yb: 22 + o, node: nodeOf(a.run), w: a.w * s + 20 }; });
      var multi = pts.some(function (p) { return p.yb !== pts[0].yb; }); // rows joined by the trunk at x = 8
      pts.forEach(function (p) { var d = 1e9; pts.forEach(function (q) { if (q !== p && q.yb === p.yb) d = Math.min(d, Math.abs(q.x - p.x)); }); p.w = Math.min(p.w, d - 8, 2 * Math.min(p.x - (multi ? 12 : 0), z.w - p.x) - 4); });
      pillsAndBus(c, pts, 22, t);
      pts.forEach(function (p) {
        var n = p.node, txt = n.n + ' · ' + n.role + (n.me ? ' · dieses Gerät' : n.unk ? ' · unbekannt' : ''), w;
        c.font = '600 11px sans-serif';
        if (c.measureText(txt).width + 18 > p.w) txt = n.n + ' · ' + n.role;
        w = Math.min(c.measureText(txt).width + 18, p.w);
        nodeBox(c, p, p.x - w / 2, p.y, w, 22, [], p.arr);
        c.font = '600 11px sans-serif'; c.fillStyle = n.timeout ? BADFG : INK; c.textAlign = 'center'; c.fillText(fit(c, txt, w - 10), p.x, p.y + 11);
      });
    }
  }
  function drawCube(c, z, t) {
    var n = lay.p.n, run = lay.runs[0], w = z.w, h = z.h, cb = plan.cube, m = (n - 1) / 2;
    if (cb.auto) cb.yaw = .6 + t * .3;
    var sp = Math.min(w, h - 30) / (n * 2.15) * cb.zoom, D = n * 3, cy = Math.cos(cb.yaw), sy = Math.sin(cb.yaw), cp = Math.cos(cb.pitch), spp = Math.sin(cb.pitch);
    function P(x, y, zz) {
      var X = x - m, Y = y - m, Z = zz - m, x1 = X * cy - Z * sy, z1 = X * sy + Z * cy, y1 = Y * cp + z1 * spp, z2 = -Y * spp + z1 * cp, f = D / (D + z2);
      return [w / 2 + x1 * sp * f, (h - 14) / 2 - y1 * sp * f, z2, f];
    }
    var fl = P(m, -1.2, m), gr = c.createRadialGradient(fl[0], fl[1], 2, fl[0], fl[1], sp * n * .8); // soft floor shadow
    gr.addColorStop(0, 'rgba(60,70,60,.13)'); gr.addColorStop(1, 'rgba(60,70,60,0)');
    c.fillStyle = gr; c.save(); c.translate(fl[0], fl[1]); c.scale(1, .32); c.translate(-fl[0], -fl[1]); c.beginPath(); c.arc(fl[0], fl[1], sp * n * .8, 0, 6.283); c.fill(); c.restore();
    var e = -.5, E = n - .5, C = [[e, e, e], [E, e, e], [E, E, e], [e, E, e], [e, e, E], [E, e, E], [E, E, E], [e, E, E]].map(function (q) { return P(q[0], q[1], q[2]); });
    c.strokeStyle = '#c3c8c0'; c.lineWidth = 1;
    [[0, 1], [1, 2], [2, 3], [3, 0], [4, 5], [5, 6], [6, 7], [7, 4], [0, 4], [1, 5], [2, 6], [3, 7]].forEach(function (q) { c.beginPath(); c.moveTo(C[q[0]][0], C[q[0]][1]); c.lineTo(C[q[1]][0], C[q[1]][1]); c.stroke(); });
    var pts = [], x, y, zz, idx;
    for (zz = 0; zz < n; zz++) for (y = 0; y < n; y++) for (x = 0; x < n; x++) {
      idx = zz * n * n + y * n + (y & 1 ? n - 1 - x : x);                          // ROWS_SERPENTINE_3D
      var p = P(x, y, zz); p.push(runPx(run, run.rev ? n * n * n - 1 - idx : idx, t)); pts.push(p);
    }
    pts.sort(function (a, b) { return b[2] - a[2]; });
    var r0 = sp * .17;
    pts.forEach(function (p) {
      var col = p[4], r = r0 * p[3], depth = clamp(.5 - p[2] / (n * 1.8), 0, 1);
      if (!col || col[3] < .05) { c.globalAlpha = .35 + .55 * depth; c.fillStyle = rgb(OFF); c.beginPath(); c.arc(p[0], p[1], r * .8, 0, 6.283); c.fill(); c.strokeStyle = OFFE; c.lineWidth = .6; c.stroke(); c.globalAlpha = 1; return; }
      var mm = mix(OFF, col, clamp(col[3], 0, 1));
      c.globalAlpha = .16 * col[3]; c.fillStyle = halo(col); c.beginPath(); c.arc(p[0], p[1], r * 2.1, 0, 6.283); c.fill();
      c.globalAlpha = .55 + .45 * depth; c.fillStyle = rgb(mm); c.beginPath(); c.arc(p[0], p[1], r * 1.15, 0, 6.283); c.fill(); if (pale(mm)) { c.strokeStyle = 'rgba(150,115,60,.5)'; c.lineWidth = .8; c.stroke(); }
      c.globalAlpha = 1;
    });
    c.font = '11px sans-serif'; c.fillStyle = FAINT; c.textAlign = 'left'; c.textBaseline = 'alphabetic';
    c.fillText(plan.cv && plan.cv.id !== 'plan' ? 'dreht sich mit der Draufsicht' : cb.auto ? 'dreht sich · ziehen zum Drehen' : 'Doppelklick: Ansicht zurücksetzen', 12, h - 12);
  }
  (function cubeInput() {
    var cv = $('plan'), drag = null;
    function isCube() { return !plan.G && TPL[lay.tpl].cube; }
    cv.addEventListener('pointerdown', function (e) { if (!isCube()) return; plan.cube.auto = false; drag = [e.clientX, e.clientY]; cv.setPointerCapture(e.pointerId); cv.style.cursor = 'grabbing'; });
    cv.addEventListener('pointermove', function (e) {
      if (!drag) return; var cb = plan.cube;
      cb.yaw += (e.clientX - drag[0]) * .01; cb.pitch = clamp(cb.pitch + (e.clientY - drag[1]) * .01, -1.3, 1.3); drag = [e.clientX, e.clientY];
    });
    function up() { if (drag) { drag = null; cv.style.cursor = 'grab'; } }
    cv.addEventListener('pointerup', up); cv.addEventListener('pointercancel', up);
    cv.addEventListener('dblclick', function () { if (isCube()) plan.cube = { yaw: .6, pitch: .45, zoom: 1, auto: true }; });
    cv.addEventListener('wheel', function (e) { if (!isCube()) return; e.preventDefault(); plan.cube.zoom = clamp(plan.cube.zoom * Math.exp(-e.deltaY * .0012), .6, 2.2); }, { passive: false });
  })();

  // ---- chain canvas -------------------------------------------------------------------------------
  function layoutChain() { var cv = $('chaincv'); if (cv) plan.csz = sizeCanvas(cv, 268); }
  function drawChain(t) {
    var cv = $('chaincv'); if (!cv || !plan.csz) return;
    var c = cv.getContext('2d'), z = plan.csz, ch = sc.chain, cs = sc.segs[ch.seg], pad = 16, bw = z.w - 2 * pad, susp = bandSuspended(cs);
    c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = STAGE; c.fillRect(0, 0, z.w, z.h);
    var yBus = 26, yNode = 56, nh = 52, yBand = 156, bh = 64, H2 = is2D(cs.geo) && ch.total % cs.geo[1] === 0 ? cs.geo[1] : 0, gapX = susp ? 8 : 1;
    var pts = ch.nodes.map(function (n) { var x0 = pad + n.off / ch.total * bw, x1 = pad + (n.off + n.len) / ch.total * bw; return { x: (x0 + x1) / 2, x0: x0, x1: x1, y: yNode, node: n }; });
    pillsAndBus(c, pts, yBus, t);
    // The sync payload (sent by this master or received by this slave) is the state the whole band follows:
    // the other parts are drawn from it, marked as derived - nothing from those devices is measured here.
    var spl = cs.ch.pl, splOn = spl && (spl[0] & 1), splTxt = spl ? fxName(spl[1]) + ' · ' + (splOn ? 'Ein' : 'Aus') + ' · ' + Math.round(spl[7] * 100 / 255) + ' %' : '';
    pts.forEach(function (p) {
      var n = p.node, w = Math.min(p.x1 - p.x0 - 10, 250), last;
      if (n.me) last = n.timeout ? 'kein Sync seit ' + cs.ch.age + ' s' : n.wait ? 'Überwachung ab Startende' : cs.ch.none ? (n.role === 'Master' ? 'noch nichts gesendet' : 'noch kein Sync empfangen') : susp ? 'EM ' + cs.em + ' läuft · Band ausgesetzt' : (n.role === 'Master' ? 'Sync gesendet vor ' : 'Sync empfangen vor ') + cs.ch.age + ' s';
      else last = spl ? 'laut Sync: ' + splTxt : n.unk ? 'Zustand unbekannt' : 'meldet nichts zurück';
      nodeBox(c, p, p.x - w / 2, yNode, w, nh, [n.n, n.role + (n.me ? ' · dieses Gerät' : ''), last], p.arr);
      c.strokeStyle = n.me ? GREEN : LINE; c.lineWidth = 1; c.setLineDash([2, 3]); c.beginPath(); c.moveTo(p.x, yNode + nh); c.lineTo(p.x, yBand - 4); c.stroke(); c.setLineDash([]);
      var lit = n.me ? cs.st !== 'off' && cs.st !== 'wait' && (!!sc.bar.power || cs.st === 'lock') : false, x0 = p.x0 + gapX, x1 = p.x1 - gapX;
      if (!n.me) {
        c.save(); rr(c, x0, yBand, x1 - x0, bh, 4); c.clip(); c.fillStyle = '#f3f4f1'; c.fillRect(x0, yBand, x1 - x0, bh);
        c.strokeStyle = '#e3e6e0'; c.lineWidth = 6; for (var hx = x0 - bh; hx < x1; hx += 14) { c.beginPath(); c.moveTo(hx, yBand + bh); c.lineTo(hx + bh, yBand); c.stroke(); }
        if (splOn) { c.globalAlpha = .35; c.fillStyle = rgb([spl[3], spl[4], spl[5]].every(function (v) { return !v; }) ? [235, 225, 200] : [spl[3], spl[4], spl[5]]); c.fillRect(x0, yBand + bh * .42, x1 - x0, bh * .16); c.globalAlpha = 1; }
        c.restore(); c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center';
        if (spl) { c.fillText('laut Sync: ' + splTxt, p.x, yBand + bh / 2 - 8); c.fillText('errechnet, nicht gemessen', p.x, yBand + bh / 2 + 18); }
        else c.fillText('unbekannt', p.x, yBand + bh / 2);
      } else if (H2) { // 2D: columns of the global matrix (band) or of the own matrix (EM running)
        var cols = n.len / H2, cw = (x1 - x0) / cols, rows = Math.min(16, H2 * 2);
        for (var X = 0; X < cols; X++) for (var r = 0; r < rows; r++) {
          var li = fwd(cs.geo[3], X, Math.floor(r * H2 / rows), cols, H2), col = lit ? segPx(cs, li, t, n.off) : null;
          c.fillStyle = rgb(col ? mix(OFF, col, clamp(col[3], 0, 1)) : OFF); c.fillRect(x0 + X * cw + (cw > 3 ? .5 : 0), yBand + r * bh / rows, Math.max(1, cw - (cw > 3 ? 1 : 0)), bh / rows - .6);
        }
      } else {
        var smp = Math.max(8, Math.floor((x1 - x0) / 3)), sw = (x1 - x0) / smp;
        for (var j = 0; j < smp; j++) { var col1 = lit ? segPx(cs, Math.floor(j * n.len / smp), t, n.off) : null; c.fillStyle = rgb(col1 ? mix(OFF, col1, clamp(col1[3], 0, 1)) : OFF); c.fillRect(x0 + j * sw, yBand + bh / 2 - 6, sw + .3, 12); }
      }
      if (n.me && !lit) { c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center'; c.fillText(cs.st === 'wait' ? 'noch keine Ausgabe (Startphase)' : n.timeout ? 'aus · nach Sync-Timeout' : 'aus · Gerät aus', p.x, yBand + bh / 2 - 14); }
      rr(c, x0 - 1, yBand - 3, x1 - x0 + 2, bh + 6, 5); c.lineWidth = n.me ? 2 : 1; c.strokeStyle = n.me ? GREEN : LINE; c.setLineDash(n.me ? [] : [4, 3]); c.stroke(); c.setLineDash([]);
      c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center';
      var lab = susp ? (H2 ? 'eigene Matrix ' + n.len / H2 + ' × ' + H2 : 'nur eigene LEDs ' + n.len) : 'LED ' + n.off + '–' + (n.off + n.len - 1) + (H2 ? ' · Spalten ' + n.off / H2 + '–' + ((n.off + n.len) / H2 - 1) : '');
      c.fillText(fit(c, lab, x1 - x0 - 6), p.x, yBand + bh + 16);
    });
    c.fillStyle = FAINT; c.textAlign = 'left'; c.fillText(susp ? 'Band ausgesetzt' : '0', pad, yBand + bh + 34); c.textAlign = 'right';
    c.fillText(susp ? 'jedes Gerät für sich, keine gemeinsame Fläche' : fmt(ch.total) + ' LEDs gesamt' + (H2 ? ' · Matrix ' + ch.total / H2 + ' × ' + H2 : ''), pad + bw, yBand + bh + 34);
  }

  // ---- HTML renderers -----------------------------------------------------------------------------
  function stPill(s) { if (s.st === 'on' && !s.bri) return pill('idle', 'Aus · 0 %'); var m = { on: ['ok', 'Ein'], off: ['idle', 'Aus'], pause: ['warn', 'Pausiert'], lock: ['bad', 'Gesperrt'], wait: ['idle', 'wartet auf Startende'] }[s.st]; return pill(m[0], m[1]); }
  function geoShort(g) { return is1D(g) ? '1D' : is2D(g) ? g[0] + '×' + g[1] + ' 2D' : g[0] + '×' + g[1] + '×' + g[2] + ' 3D'; }
  function emText(s) {
    if (!s.em) return '<span class="np-f">—</span>';
    var e = emOf(s.em), q = e && e.cues[(s.cue || 1) - 1];
    if (!e) return 'EM ' + s.em + '<small>nicht in der Gerätekonfiguration</small>';
    var st = { run: 'Cue ' + s.cue + '/' + e.cues.length + ' „' + (q ? q.n : '') + '“', pause: 'pausiert bei Cue ' + s.cue, susp: 'angehalten (Gerät aus) · Cue ' + s.cue, end: 'nicht aktiv', stop: 'nicht aktiv' }[s.ems] || ''; // firmware runState: 0 = stopped, 1 = running, 2 = paused
    return '<b>EM ' + e.id + '</b><small>' + st + '</small>';
  }
  function swatch(c1, w, cw) {
    var c = rgbOf(c1), d = shown(c[0], c[1], c[2], w, cw, sc.bar.layout);
    return '<span class="np-sw" style="background:' + rgb(d) + '" title="RGB ' + c1.toUpperCase() + (w ? ' + W ' + w : '') + (cw ? ' + CW ' + cw : '') + '"></span>';
  }

  function renderBar() {
    var b = sc.bar, P = power, pw = sc.pw, notes = b.notes.slice(), cls, curTxt = '';
    if (P && P.active) notes.push('Begrenzung aktiv');
    cls = !b.power ? 'bad' : notes.length ? 'warn' : '';
    $('bar').className = 'np-bar ' + cls;
    if (P) {
      if (pw.mode !== 'off') curTxt = meter(P.load, P.active ? 'warn' : '', pw.thr) + '<b>' + fmt(P.gMa) + ' mA</b><em>/ ' + fmt(pw.lim) + ' mA · ' + P.load + ' %</em>';
      else curTxt = '<b>' + fmt(P.total) + ' mA</b><em>keine globale Grenze</em>';
    }
    $('bar').innerHTML = '<div class="np-b"><span>Gerät</span>' + (b.power ? pill('ok', 'Ein') : pill('bad', 'Aus')) + '</div>' +
      '<div class="np-b"><span>Helligkeit</span><b>' + b.bri + ' %</b></div>' +
      '<div class="np-b"><span>LEDs</span><b>' + fmt(sc.strips.reduce(function (a, s) { return a + s.leds; }, 0)) + '</b><em>' + sc.strips.length + ' Streifen · ' + b.layout + '</em></div>' +
      '<div class="np-b"><span>Bildrate</span><b class="' + (b.fps && b.fps < b.fpsT * .8 ? 'np-warn' : '') + '">' + (b.fps || '—') + '</b><em>/ ' + b.fpsT + ' fps</em></div>' +
      '<div class="np-b"><span>Strom</span>' + curTxt + '</div>' +
      '<div class="np-b"><span>HCL</span><b>' + (b.hcl ? fmt(b.hcl) + ' K' : '<span class="np-f">aus</span>') + '</b></div>' +
      '<div class="np-b notes"><span>Hinweise</span>' + (notes.length ? notes.map(function (n) { return pill(/aus|≠|Sync|Fehler/.test(n) ? 'bad' : n === 'Verbinde …' ? 'idle' : 'warn', n); }).join('') : pill('ok', 'keine')) + '</div>';
  }

  function renderPipe() {
    var tot = virtLeds(), h = '', a = 0;
    function row(l, sub, parts) { return '<div class="np-pipe-l"><b>' + l + '</b><span>' + sub + '</span></div><div class="np-row">' + parts + '</div>'; }
    function part(w, cls, t1, t2, tip) { return '<div class="' + cls + '" style="flex:' + w + ' 1 0" title="' + (tip || '') + '">' + t1 + '<small>' + t2 + '</small></div>'; }
    h += row('Physikalische Streifen', sc.strips.length + ' Anschlüsse', sc.strips.map(function (s, i) { return part(s.leds, 't' + (i % 8), '<b>S' + (i + 1) + '</b> ' + stripLabel(i), s.leds + ' LEDs', s.chip + ' an ' + s.port); }).join(''));
    h += '<div class="np-arrow">↓ hintereinander gehängt</div>';
    // Order and gaps as in the device: a strip the firmware left off keeps its ETS range unlit
    var vparts = sc.strips.map(function (s, i) { return { s: s, i: i, v: stripStart(i) }; }).filter(function (e) { return !sc.bar.vleds || e.s.vs >= 0; }).sort(function (x, y) { return x.v - y.v; }).map(function (e) {
      var r = (e.v > a ? part(e.v - a, 'peer', a + '–' + (e.v - 1), 'ohne Streifen') : '') + part(e.s.leds, 't' + (e.i % 8), e.v + '–' + (e.v + e.s.leds - 1), 'S' + (e.i + 1)); a = Math.max(a, e.v + e.s.leds); return r;
    }).join('') + (tot > a ? part(tot - a, 'peer', a + '–' + (tot - 1), 'ohne Streifen') : '');
    h += row('Virtueller Streifen', fmt(tot) + ' LEDs · ' + sc.bar.layout, vparts);
    h += '<div class="np-arrow">↓ aufgeteilt in Segmente</div>';
    h += row('Segmente', sc.segs.length + (sc.segs.length > 1 ? ' Segmente' : ' Segment'), sc.segs.map(function (s, i) {
      var g = s.geo; return part(s.b - s.a + 1, 't' + SEGT(i), '<b>' + (i + 1) + '</b> ' + segLabel(i), s.a + '–' + s.b + ' · ' + geoShort(g) + (!is1D(g) ? ' · ' + TOPO[g[3]] : ''));
    }).join(''));
    if (sc.chain) {
      var ch = sc.chain, cs = sc.segs[ch.seg];
      h += '<div class="np-arrow">↓ Segment ' + (ch.seg + 1) + ' ist Teil einer Effektkette' + (bandSuspended(cs) ? ' · ausgesetzt, solange EM ' + cs.em + ' läuft' : '') + '</div>';
      h += row('Effektkette', fmt(ch.total) + ' LEDs über alle Geräte', ch.nodes.map(function (n) { return part(n.len, n.me ? 't' + NODET[ch.nodes.indexOf(n) % 4] + ' me' : 'peer', n.n + (n.me ? ' · dieses Gerät' : ''), n.off + '–' + (n.off + n.len - 1)); }).join(''));
    }
    setH($('pipe'), h);
  }

  function ledLegend() {
    var ring = 'box-shadow:inset 0 0 0 1px rgba(150,115,60,.65)';
    function k(bg, txt, st) { return '<span class="np-key"><i style="background:' + bg + ';' + (st || '') + '"></i>' + txt + '</span>'; }
    return '<span><b>LED-Farbe = Ausgabe:</b></span>' + k('linear-gradient(90deg,#ff4a3a,#3bd16a,#3f7bff)', 'RGB wie ausgegeben') +
      (sc.bar.layout === 'RGBW' ? k(rgb(TW), 'W', ring) : sc.bar.layout === 'RGBCCT' ? k(rgb(TWW), 'WW', ring) + k(rgb(TCW), 'CW', ring) : '') + (sc.bar.layout !== 'RGB' ? '<span>Weiß additiv im Weißton des Streifens</span>' : '') +
      k('#ffb54f', 'Helligkeit = Intensität mit Schein', 'box-shadow:0 0 0 3px rgba(255,181,79,.3)') + k(rgb(OFF), 'aus', 'box-shadow:inset 0 0 0 1px ' + OFFE);
  }
  function renderCap() {
    var T = TPL[lay.tpl], h = ledLegend();
    if (T.cube) h += '<span><b>Ziehen</b> dreht · <b>Doppelklick</b> setzt zurück · <b>Mausrad</b> zoomt</span>';
    if (plan.chained) h += '<span class="np-key"><i style="background:' + GREEN + '"></i>Sync-Telegramm vom Master</span>';
    setH($('plan-cap'), h);
    $('lay-name').textContent = T.name + (lay.saved ? ' · in diesem Browser gespeichert' : ' · Standard');
  }

  function renderSegs() {
    var h = '<table class="np-tbl"><tr><th>Segment</th><th>Bereich</th><th>Form</th><th>Zustand</th><th>Hell.</th><th>Farbe</th><th>Effekt</th><th>Effektmanager</th><th>Kette</th><th>HCL</th></tr>';
    sc.segs.forEach(function (s, i) {
      var ch = '<span class="np-f">—</span>', lk = lookOf(s);
      if (s.ch.m) ch = (s.ch.timeout ? pill('bad', 'Slave · Timeout') : bandSuspended(s) ? (s.ch.m === 1 ? 'Master' : 'Slave') + ' ' + pill('warn', 'ausgesetzt (EM)') : (s.ch.m === 1 ? 'Master' : 'Slave')) + '<small>' + fmt(s.ch.total) + ' LEDs · Teil ' + s.ch.off + '–' + (s.ch.off + s.b - s.a) + '</small>';
      h += '<tr><td>' + badge(SEGT(i), i + 1) + segName(i) + '</td><td class="np-num">' + s.a + '–' + s.b + '<small>' + (s.b - s.a + 1) + ' LEDs</small></td><td>' + geoShort(s.geo) + (!is1D(s.geo) ? '<small>' + TOPO[s.geo[3]] + '</small>' : '') + '</td>' +
        '<td>' + stPill(s) + '</td><td class="np-num">' + s.bri + ' %</td>' +
        '<td>' + swatch(s.c1, s.w, s.cw) + (s.c2 ? swatch(s.c2) : '') + '<small>' + s.c1.toUpperCase() + (s.w ? ' + W ' + s.w : '') + (s.cw ? ' · CW ' + s.cw : '') + '</small></td>' +
        '<td>' + (emOn(s) ? lk.fx + '<small>aus Cue ' + s.cue + '</small>' : s.fx + (s.scenes ? '<small>Szenen ' + s.scenes.map(function (q) { return q.n; }).join(', ') + ' angelegt</small>' : '')) + '</td><td>' + emText(s) + '</td><td>' + ch + '</td><td>' + (s.hcl ? fmt(s.hcl) + ' K<small>' + (s.hclSrc || '') + '</small>' : '<span class="np-f">—</span>') + '</td></tr>';
    });
    h += '</table><div class="np-legend"><span>' + pill('ok', 'Ein') + '</span><span>' + pill('warn', 'Pausiert') + ' EM angehalten</span><span>' + pill('bad', 'Gesperrt') + ' Sperre-KO aktiv, ignoriert Befehle</span><span>' + pill('idle', 'Aus') + '</span>' + (sc.segs.some(function (s) { return s.st === 'wait'; }) ? '<span>' + pill('idle', 'wartet auf Startende') + ' noch keine Ausgabe</span>' : '') + '<span>Farbfeld = eingestellte Farbe, Weiß im Weißton des Streifens</span></div>';
    if (sc.segs.some(function (s) { return s.st === 'lock'; })) h += '<div class="np-note warn"><b>Gesperrt:</b> Ein gesperrtes Segment ignoriert alle steuernden KOs und Sync-Telegramme, bis die Sperre aufgehoben wird. Effekt und EM laufen weiter, auch wenn das Gerät ausgeschaltet wird: die Sperre hat Vorrang vor Aus.</div>';
    setH($('segs'), h);
  }

  function cueEl(s) { // elapsed seconds in the active cue (device: EffektManagerController::cueElapsedMs)
    return s.cueEl + (s.ems === 'run' ? (performance.now() - LIVE.rxAt) / 1000 : 0);
  }
  function renderEm() {
    if (LIVE.emSum < 0 && sc.emCount) { setH($('em'), '<div class="np-empty">Effektmanager werden geladen …</div>'); return; }
    if (!sc.ems.length) { setH($('em'), '<div class="np-empty"><b>Kein Effektmanager angelegt.</b><br>Ein Effektmanager spielt Cues (Effekt, Farbe, Dauer, Überblendung) nacheinander auf einem Segment ab.</div>'); return; }
    var h = '<div class="np-em-k"><span><i></i>feste Dauer (Breite ≈ Dauer)</span><span><i class="hold"></i>halten: bleibt stehen, weiter nur per KO</span><span><i class="done"></i>bis Effekt fertig</span><span><i class="fade"></i>Überblendung ↘↗ (aus- und einblenden)</span><span><i class="act"></i>hier steht ein Segment gerade</span></div>';
    sc.ems.forEach(function (e) {
      var users = []; sc.segs.forEach(function (s, i) { if (s.em === e.id) users.push([i, s]); });
      var stp = [pill('idle', 'deaktiviert'), pill('ok', 'aktiv'), pill('warn', 'suspendiert')][e.st], blocked = 0, t0 = 0, open = false;
      e.cues.forEach(function (q, k) { if (q.d === 0 && !blocked) blocked = k + 1; });
      var end = (blocked ? 'Cue ' + blocked + ' hält · ' : '') + (e.loop ? '↺ Loop' : e.next ? '→ danach EM ' + e.next : '■ Ende');
      h += '<div class="np-em' + (e.st === 0 ? ' dis' : '') + '"><div class="np-em-h"><b>EM ' + e.id + '</b>' + stp + '<span class="np-m">' + e.cues.length + ' Cues · ' + end + '</span><span class="np-sp"></span>' +
        (users.length ? '<span class="np-m">auf</span> ' + users.map(function (u) { return badge(SEGT(u[0]), u[0] + 1) + segLabel(u[0]); }).join(' ') : '<span class="np-f">auf keinem Segment</span>') + '</div>';
      h += '<div class="np-tlw"><div class="np-tl">';
      e.cues.forEach(function (q, k) {
        var hold = q.d === 0, done = q.d === 0xFFFF, grow = hold || done ? 70 : q.d, cls = hold ? ' hold' : done ? ' done' : '';
        var here = users.filter(function (u) { return u[1].cue === k + 1 && /run|pause|susp/.test(u[1].ems); }), running = here.some(function (u) { return u[1].ems === 'run'; });
        if (here.length) cls += running ? ' act' : ' paused';
        h += '<div class="np-cue t' + CUET(k) + cls + '" style="flex-grow:' + grow + '"><b>' + (k + 1) + ' · ' + q.n + '</b><small>' + q.fx + (q.bri ? ' · ' + q.bri + ' %' : '') + '</small>' +
          '<small>' + (open ? 'Start per KO' : 'ab ' + tfmt(t0)) + ' · ' + dur(q.d) + (hold ? '' : q.f ? ' · ↘↗ ' + q.f / 1000 + ' s' : ' · harter Schnitt') + '</small>';
        if (here.length) {
          var el = cueEl(here[0][1]);
          var runL = here.filter(function (u) { return u[1].ems === 'run'; }).map(function (u) { return u[0] + 1; }), pauL = here.filter(function (u) { return u[1].ems !== 'run'; }).map(function (u) { return u[0] + 1; });
          h += '<span class="np-now">' + (runL.length ? '▶ Seg ' + runL.join(', ') + ' läuft' : '') + (runL.length && pauL.length ? ' · ' : '') + (pauL.length ? '⏸ Seg ' + pauL.join(', ') + ' pausiert' : '') + ' · ' + (q.d && !done ? mmss(Math.min(el, q.d)) + ' / ' + mmss(q.d) : 'seit ' + mmss(el)) + '</span>';
          if (q.d && !done) h += '<span class="np-prog" title="verstrichene Zeit in der Cue"><i style="width:' + clamp(el / q.d * 100, 0, 100) + '%"></i></span>';
        }
        if (q.f && !hold) h += '<span class="np-fade" style="width:' + clamp(done ? 14 : q.f / 10 / q.d, 7, 30) + '%"></span>';
        h += '</div>';
        if (hold || done) open = true; else t0 += q.d + q.f / 2000; // the next cue is applied halfway through the fade
      });
      h += '<div class="np-tail">' + (e.loop ? '↺ zu Cue 1' : e.next ? '→ EM ' + e.next : '■ Ende') + '</div></div>';
      if (e.loop && !blocked) h += '<div class="np-loop" style="right:46px"></div>';
      h += '</div>';
      if (e.st === 2) h += '<div class="np-note warn">Suspendiert: Der EM ist konfiguriert, startet aber nicht, bis er wieder aktiv geschaltet wird.</div>';
      if (blocked) h += '<div class="np-note">Cue ' + blocked + ' hält (Dauer 0): weiter geht es nur per KO „Cue setzen“ oder „Cue +“; ein solcher Sprung ist ein harter Wechsel ohne Überblendung.</div>';
      h += '</div>';
    });
    var chs = sc.chain && sc.segs[sc.chain.seg];
    h += '<div class="np-note"><b>Position innerhalb der Cue</b> (Zeit und Balken): verstrichene Zeit in der laufenden Cue, beim Pausieren eingefroren.' +
      ' „ab“ zählt die Überblendung mit: die nächste Cue startet nach der halben Überblendzeit.' +
      (chs ? '<br><b>Effektkette:</b> Solange ein EM auf dem Segment läuft, ist die Kette dort ausgesetzt: das Gerät zeigt den EM nur auf seiner eigenen ' + (is2D(chs.geo) ? 'Matrix ' + chs.geo[0] + ' × ' + chs.geo[1] : 'LED-Reihe') + '. Cues gehen nicht über die Kette; jedes Gerät braucht seinen eigenen EM.' : '') + '</div>';
    setH($('em'), h);
  }

  function renderChain() {
    var ch = sc.chain;
    var rules = '<div class="np-note info"><b>Effektkette und Effektmanager.</b> Läuft auf einem Segment ein EM, setzt der EM die Kette an diesem Segment aus: das Gerät rendert nur seine eigene Matrix bzw. LED-Reihe, nicht seinen Teil der gemeinsamen Fläche. Nach dem EM-Stop gilt die Kette wieder.' +
      '<br><b>Master mit laufendem EM:</b> sendet weiter den Zustand von vor dem EM-Start (Effekt, Tempo, Farbe); Ein/Aus und Helligkeit bleiben live. Cue-Wechsel gehen nicht über die Kette.' +
      '<br><b>Slave mit eigenem laufendem EM:</b> ignoriert die Sync-Telegramme. Jedes Telegramm zählt aber als Lebenszeichen, der Sync-Timeout greift also nicht. Ein Slave ohne eigenen EM übernimmt den Zustand des Masters und rendert ihn auf der gemeinsamen Fläche.</div>';
    if (!ch) { setH($('chain'), '<div class="np-empty"><b>Kein Segment in einer Effektkette.</b><br>In einer Effektkette spielen mehrere Geräte einen gemeinsamen Effekt: Jedes Gerät rendert seinen Teil eines gemeinsamen Bands, der Master sendet Ein/Aus, Effekt, Tempo, Farbe und Helligkeit an die Slaves.</div>' + rules); return; }
    var s = sc.segs[ch.seg], me = ch.nodes.filter(function (n) { return n.me; })[0], H2 = is2D(s.geo) && ch.total % s.geo[1] === 0 ? s.geo[1] : 0, h = '', susp = bandSuspended(s);
    if (!me) { setH($('chain'), '<div class="np-empty">Kettendaten unvollständig.</div>'); return; }
    if (susp) h += '<div class="np-note warn"><b>Kette ausgesetzt:</b> EM ' + s.em + ' läuft auf diesem Gerät. Es zeigt den EM nur auf seiner eigenen ' + (H2 ? 'Matrix ' + s.geo[0] + ' × ' + s.geo[1] + ', nicht auf der gemeinsamen Fläche ' + ch.total / H2 + ' × ' + H2 : 'LED-Reihe') + '. ' +
      (me.role === 'Master' ? 'Die Slaves bekommen weiter den Zustand von vor dem EM-Start; ohne eigenen EM zeigen sie diesen Zustand auf der gemeinsamen Fläche.' : 'Sync-Telegramme vom Master werden ignoriert, halten aber die Timeout-Überwachung am Leben.') + '</div>';
    if (me.wait) h += '<div class="np-note info"><b>Startphase:</b> Die Sync-Überwachung beginnt erst nach der Startverzögerung; bis dahin zählt kein Timeout.</div>';
    if (me.timeout) h += '<div class="np-note bad"><b>Kein Sync vom Master ' + (s.ch.none ? 'seit dem Start' : 'seit ' + s.ch.age + ' s') + '</b> (Timeout ' + s.ch.to * 10 + ' s). Das Segment „' + segLabel(ch.seg) + '“ wurde ausgeschaltet; das nächste Sync-Telegramm schaltet es wieder nach Vorgabe des Masters.</div>';
    h += '<div class="np-card"><div class="np-card-h"><h3>' + segLabel(ch.seg) + ' · ' + me.role + '</h3>' + (me.timeout ? pill('bad', 'Timeout') : me.wait ? pill('idle', 'Startphase') : susp ? pill('warn', 'ausgesetzt (EM ' + s.em + ')') : pill('ok', me.role === 'Master' ? 'sendet' : 'empfängt')) + '<span class="np-sp"></span><span class="np-f">Namen und Lage der anderen Geräte stammen aus der Anordnung</span></div><canvas class="np-stage" id="chaincv"></canvas>' +
      '<div class="np-cap">' + ledLegend() + '</div><div class="np-cap"><span class="np-key"><i style="background:' + GREEN + '"></i>Sync-Telegramm alle 3 s (Lebenszeichen), bei Änderung sofort</span><span>grün umrandet = dieses Gerät</span><span>gestrichelt = andere Geräte, von hier nicht überwacht</span></div></div>';
    var pl = s.ch.pl, plTxt = pl ? fxName(pl[1]) + ' ' + hex6(pl[3] << 16 | pl[4] << 8 | pl[5]).toUpperCase() + (pl[6] ? ' + W ' + pl[6] : '') + (pl[8] ? ' + CW ' + pl[8] : '') + ', ' + (pl[0] & 1 ? 'Ein' : 'Aus') + ' · ' + Math.round(pl[7] * 100 / 255) + ' %' : '';
    var sent = me.role === 'Master' ? (s.ch.none ? 'noch nichts gesendet' : 'vor ' + s.ch.age + ' s · ' + (susp ? 'Zustand vor dem EM-Start: ' : '') + plTxt) : '';
    h += '<div class="np-two"><div class="np-card"><div class="np-card-h"><h3>Was dieses Gerät weiß</h3></div><div class="np-card-b np-kv">' +
      '<i>Rolle</i><b>' + me.role + (me.role === 'Slave' ? ' · ' + ['Sync übernimmt immer', 'Lokal hat Vorrang'][s.ch.pol] : '') + '</b>' +
      '<i>Gesamtlänge (konfiguriert)</i><b>' + fmt(ch.total) + ' LEDs</b><i>Eigener Versatz (konfiguriert)</i><b>' + s.ch.off + ' · Teil ' + s.ch.off + '–' + (s.ch.off + s.b - s.a) + '</b>' +
      (H2 ? '<i>2D über die Kette</i><b>gemeinsame Matrix ' + ch.total / H2 + ' × ' + H2 + ' · eigene Spalten ' + s.ch.off / H2 + '–' + ((s.ch.off + s.b - s.a + 1) / H2 - 1) + '</b>' : '') +
      '<i>Band</i><b>' + (susp ? pill('warn', 'ausgesetzt') + ' solange EM ' + s.em + ' läuft' + (H2 ? ' · eigene Matrix ' + s.geo[0] + ' × ' + s.geo[1] : '') : 'aktiv') + '</b>' +
      (me.role === 'Slave' ? '<i>Timeout</i><b>' + (s.ch.to ? s.ch.to * 10 + ' s' : 'aus') + '</b><i>Letzter Sync</i><b class="' + (me.timeout ? 'np-bad' : '') + '">' + (me.wait || s.ch.none ? 'noch keiner seit dem Start' : 'vor ' + s.ch.age + ' s' + (plTxt ? ' · ' + plTxt : '')) + '</b>' : '<i>Zuletzt gesendet</i><b>' + sent + '</b>') +
      '<i>Andere Geräte</i><b class="np-m">' + (me.role === 'Master' ? 'unbekannt: Slaves senden nichts zurück' : 'unbekannt: nur Telegramme des Masters kommen an') + '</b></div></div>' +
      '<div class="np-card"><div class="np-card-h"><h3>Was über die Kette geht</h3></div><div class="np-card-b" style="font-size:.84em">Sync-KO: 14 Byte (DPT 16), davon 9 belegt:<div class="np-bytes">' +
      ['Ein/Aus', 'Effekt', 'Tempo', 'Rot', 'Grün', 'Blau', 'Weiß (WW)', 'Helligkeit', 'Kaltweiß (CW)'].map(function (b, i) { return '<span class="t' + [0, 1, 1, 2, 2, 2, 2, 3, 2][i] + '">' + b + '</span>'; }).join('') + '</div>' +
      '<div class="np-m">Nicht übertragen: Zeit bzw. Phase, konfigurierte Effekt-Parameter (z. B. Farbton bei Cylon) und die Cues eines EM. Jedes Gerät rechnet das Band mit eigener Uhr und eigenen Parametern.</div></div></div></div>' + rules;
    setH($('chain'), h);
  }

  function renderStrips() {
    var P = power, pw = sc.pw, h = '';
    if (!P) return;
    var spi = sc.strips.some(function (s) { return s.spi; }), own = sc.strips.some(stripLimit), ownS = sc.strips.filter(stripLimit)[0];
    h += '<div class="np-card"><div class="np-card-h"><h3>Strom</h3>' + (P.active ? pill('warn', 'Begrenzung aktiv') : pill('ok', 'keine Begrenzung')) + '<span class="np-sp"></span><span class="np-f">aus dem Ausgabepuffer berechnet (nach HCL und Gamma), nicht gemessen</span></div><div class="np-card-b np-kv">' +
      '<i>Globale Strombegrenzung</i><b>' + (pw.mode === 'fix' ? 'Fester Wert · ' + fmt(pw.lim) + ' mA' : pw.mode === 'led' ? 'Pro LED · ' + pw.perLed + ' mA je LED = ' + fmt(pw.lim) + ' mA' : 'Deaktiviert') + '</b>' +
      (pw.mode !== 'off' ? '<i>Gesamt</i><b>' + meter(P.load, P.active ? 'warn lg' : 'lg', pw.thr) + fmt(P.gMa) + ' mA von ' + fmt(pw.lim) + ' mA · ' + P.load + ' %</b>' +
        '<i>Kurve</i><b>bis ' + pw.thr + ' % der Grenze (Strich) volle Helligkeit, bis zur Grenze gleitend auf ' + pw.abl + ' % (Auto-Helligkeitsbegrenzung); darüber 95 % der Grenze, höchstens ' + pw.abl + ' %</b>'
        : '<i>Gesamt</i><b>' + fmt(P.total) + ' mA <span class="np-f">ohne globale Grenze; das KO „Stromverbrauch Gesamt“ meldet 0</span></b>' +
          (ownS ? '<i>Kurve je Streifen</i><b>bis ' + ownS.thr + ' % der eigenen Grenze volle Helligkeit, bis zur Grenze gleitend auf ' + ownS.abl + ' %; darüber 95 % der Grenze, höchstens ' + ownS.abl + ' %</b>' : '')) +
      '<i>Begrenzung aktiv</i><b class="np-m">wenn der Begrenzer in den letzten 5 s mindestens ein Bild gedimmt hat</b>' +
      '<i>KOs</i><b class="np-m">Stromverbrauch Gesamt und je Streifen als DPT 7.012 (mA)</b></div></div>';
    h += '<div class="np-tw"><table class="np-tbl"><tr><th>Streifen</th><th>Anschluss</th><th>Chip</th><th>LEDs</th><th>Farbfolge</th><th>Treiber</th><th>Timing</th><th>Strom</th><th>Begrenzung</th></tr>';
    sc.strips.forEach(function (s, i) {
      var p = P.strips[i], lim = p.lim, ref = lim || (pw.mode !== 'off' ? pw.lim : 0), pct = ref ? Math.round(p.ma / ref * 100) : 0;
      var mode = s.pm === 'none' ? 'keine<small>dieser Streifen wird nicht begrenzt</small>' : s.pm === 'global' ? (pw.mode !== 'off' ? 'Global verwenden' : 'Global verwenden<small>global deaktiviert: keine Grenze</small>') : s.pm === 'fix' ? 'Fester Wert<small>' + fmt(lim) + ' mA</small>' : 'Pro LED ' + s.perLed + ' mA<small>' + s.leds + ' × ' + s.perLed + ' = ' + fmt(lim) + ' mA</small>';
      h += '<tr><td>' + badge(i % 8, 'S' + (i + 1)) + stripLabel(i) + '</td><td>' + s.port + '</td><td>' + s.chip + '</td><td class="np-num">' + s.leds + '</td><td>' + s.order + '</td><td>' + s.drv + '</td><td class="np-num">' + s.tim + '</td>' +
        '<td class="np-num">' + (ref ? meter(pct, '') : '') + fmt(p.ma) + ' mA' + (lim ? ' / ' + fmt(lim) : '') + (s.spi ? ' ' + pill('idle', 'nicht gedimmt (SPI)') : '') + '</td><td>' + mode + '</td></tr>';
    });
    h += '</table></div><div class="np-note">' + (own ? '<b>Eigene Grenze:</b> Streifen mit „Fester Wert“ oder „Pro LED“ werden immer begrenzt, auch wenn die globale Strombegrenzung „Deaktiviert“ ist, und melden ihren Strom über ihr eigenes KO. ' : '') +
      '<b>SPI-Streifen</b> (APA102/SK9822) zählen beim Strom mit, werden aber nicht gedimmt' + (spi ? '' : ' (hier keiner angeschlossen)') + '. Berechnet wird je LED und Kanal aus dem Ausgabewert nach Gamma: „Strom pro LED“ (global, ' + (pw.perLed ? pw.perLed + ' mA' : '0 = Tabelle des LED-Typs') + ') geteilt durch die Kanäle; mit 0 gilt die Tabelle (20 mA je Kanal, APA102 15 mA).</div>';
    h += '<h2>Relais</h2>';
    if (!sc.relays.length) h += '<div class="np-empty">Kein Relais konfiguriert.</div>';
    else {
      h += '<div class="np-tw"><table class="np-tbl"><tr><th>Relais</th><th>Zustand</th><th>Logik</th><th>Pin</th><th>Ausschaltverzögerung</th><th>Mindest-Auszeit</th></tr>';
      sc.relays.forEach(function (r, i) {
        h += '<tr><td>Relais ' + (i + 1) + '</td><td>' + (r.st ? pill('ok', 'Ein') : pill('idle', 'Aus')) + '</td><td>' + (r.inv ? 'invertiert (LOW = Ein)' : 'normal (HIGH = Ein)') + '</td><td>' + (r.nopin ? pill('bad', 'kein Pin zugeordnet') : r.pin) + '</td><td>' + (r.offDelay ? r.offDelay + ' s' : '<span class="np-f">—</span>') + '</td><td>' + (r.lock ? pill('warn', 'noch ' + r.lock + ' s gesperrt') : r.minOff ? r.minOff + ' s' : '<span class="np-f">—</span>') + (r.pend >= 0 && r.pend !== undefined ? ' <span class="np-f">schaltet in ' + r.pend + ' s ' + (r.to ? 'ein' : 'aus') + '</span>' : '') + '</td></tr>';
      });
      h += '</table></div>';
    }
    setH($('strips'), h);
  }

  function renderHw() {
    var w = sc.hw, b = sc.bar, budget = b.fpsT ? 1000 / b.fpsT : 0, cpu = Math.round(w.ft / budget * 100), used = w.heapTot - w.heap, hp = Math.round(used / w.heapTot * 100), low = w.heapMin < 32;
    var bpp = { RGB: 3, RGBW: 4, RGBCCT: 5 }[b.layout] || 3, buf = virtLeds() * bpp;
    function row(l, v, wide) { return '<div class="np-r"' + (wide ? ' style="grid-column:1/-1"' : '') + '><i>' + l + '</i><b>' + v + '</b></div>'; }
    function chips(a, lab) { return '<span class="np-chips">' + a.map(function (u, i) { return '<span class="' + (u ? 'u' : '') + '">' + lab(i) + '</span>'; }).join('') + '</span>'; }
    var h = '<div class="np-grid"><div class="np-sub">Gerät</div>' + row('Prozessor', w.chip) +
      row('Hardware', w.ets ? pill('bad', 'passt nicht zur Konfiguration') + ' gefunden ' + w.board + ' (' + w.hwid + ') · konfiguriert ' + w.ets : w.board + ' <span class="np-f">' + w.hwid + ' · Pegelwandler ' + w.ls + '</span>', !!w.ets) +
      row('Build', w.build + (w.build === 'KNX TP + WLAN' ? ' <span class="np-f">KNX über TP, WLAN für Webseite, Einrichtung und OTA</span>' : '')) +
      '<div class="np-sub">Rechenzeit</div>' +
      row('Rechenzeit pro Bild (Ø seit Start)', w.ft && budget ? meter(cpu, cpu > 85 ? 'bad' : cpu > 60 ? 'warn' : '') + w.ft + ' ms von ' + budget.toFixed(1) + ' ms (' + cpu + ' %)' : '<span class="np-f">— noch kein Bild (Startphase)</span>') +
      row('Bildrate', b.fps ? b.fps + ' fps <span class="np-f">Ziel ' + b.fpsT + '</span>' + (b.fps < b.fpsT * .8 ? ' ' + pill('warn', 'unter Ziel') : '') : '<span class="np-f">— noch kein Bild</span>') +
      row('Frame-Zeit seit Start', w.ftMax ? 'min ' + w.ftMin + ' ms · max ' + w.ftMax + ' ms' : '<span class="np-f">— noch kein Bild</span>') +
      row('CPU-Temperatur', w.temp ? w.temp.toFixed(1) + ' °C' : '<span class="np-f">— noch kein Messwert</span>') +
      '<div class="np-sub">Speicher und Treiber</div>' +
      row('Arbeitsspeicher', meter(hp, low ? 'bad' : hp > 75 ? 'warn' : '') + w.heap + ' KB frei von ' + w.heapTot + ' KB <span class="np-f">' + hp + ' % belegt</span>') +
      row('Tiefster Stand', '<span class="' + (low ? 'np-bad' : '') + '">' + w.heapMin + ' KB frei</span>' + (low ? ' ' + pill('bad', 'wenig Speicher') : '')) +
      row('LED-Puffer', fmt(buf) + ' B <span class="np-f">virtueller Streifen, ' + bpp + ' B je LED</span>') +
      (!w.rmt || !w.rmt.length ? row('RMT-Kanäle (Senden)', '<span class="np-f">—</span>') : row('RMT-Kanäle (Senden)', meter(w.rmt.filter(Boolean).length / w.rmt.length * 100) + w.rmt.filter(Boolean).length + ' von ' + w.rmt.length + ' belegt <span class="np-f">ein Kanal je Streifen</span>') + row('DMA', '<span class="np-f">— nicht genutzt (RMT)</span>')) +
      '<div class="np-sub">Betrieb</div>' +
      row('Laufzeit', w.up) +
      row('Startphase', w.start === 'abgeschlossen' ? pill('ok', 'abgeschlossen') : pill('warn', 'läuft') + ' ' + w.start) +
      (w.saved ? row('Zustandsdaten', w.saved + ' <span class="np-f">Zustand aus dem Flash geladen oder Farbe/Szene seit dem Start gesetzt</span>') : '') +
      row('Fehler / Warnung', w.err ? pill('bad', w.err) : pill('ok', 'keine')) + '</div>';
    setH($('hw'), h);
  }

  // ---- control panel (concept: POST /neo/control {segment, function, value}) ----------------------
  // The device writes the value into the segment's input KO like a received telegram (processInputKo):
  // no bus telegram (valueNoSend + processInputKo in the loop task), lock / startup delay / Effektkette rules apply,
  // status KOs send as usual. Offered by the configured parameters, not by group address.
  // Effect names as in src/EffectIds.lock (IDs 0-19 1D, 20-32 2D)
  var FX1D = ['Solid', 'Wipe', 'Rainbow', 'Pride2015', 'Juggle', 'BPM', 'Cylon', 'Test', 'Fire', 'Theater Chase', 'Sparkle', 'Breathing', 'Strobe', 'Comet', 'Noise', 'Palette', 'Lightning', 'Gradient', 'Candle', 'Scroll Text'];
  var FX2D = ['Clock 2D', 'Snake 2D', 'Matrix 2D', 'Tetris 2D', 'TRON', 'Starfield Warp', 'Plasma Nebula', 'UFO Swarm', 'Game of Life 2D', 'DNA 2D', 'Aurora 2D', 'Lissajous 2D', 'Metaballs 2D'];
  // Offered by the configured parameters of the active segment (no group address needed):
  // always Ein/Aus, Helligkeit, Dimmen, Sperre, Effekt; colour by strip type; scenes 1..NEO~SceneCount;
  // EM only for EMs within NEOEMEnabled, state aktiv (NEOEM~Enabled = 1) and with cues (a start with 0 cues is refused).
  function emsFor() { return sc.ems.filter(function (e) { return e.id <= (sc.emCount || 0) && e.st === 1 && e.cues.length > 0; }); }
  function has(s, k) {
    var L = sc.bar.layout;
    switch (k) {
      case 'rgb': return L === 'RGB' || L === 'RGBCCT';
      case 'rgbw': return L === 'RGBW';
      case 'cct': return L === 'RGBCCT';
      case 'scene': return !!(s.scenes && s.scenes.length);
      case 'emStart': case 'emStop': case 'cue': return emsFor().length > 0;
      default: return true; // power, bri, dim, fx, lock
    }
  }
  var ctlLog = [], ctlHeld = false; // ctlHeld: a dim button is pressed, the panel is not rebuilt
  function ctlRow(label, body) { return '<div class="np-ctl-r"><i>' + label + '</i><div class="np-ctl-c">' + body + '</div></div>'; }
  function cbtn(i, f, v, txt, on, extra) { return '<button class="np-cb' + (on ? ' on' : '') + '" data-seg="' + i + '" data-f="' + f + '" data-v="' + v + '"' + (extra || '') + '>' + txt + '</button>'; }
  function hex2(c) { return '#' + c.map(function (x) { return ('0' + Math.round(x).toString(16)).slice(-2); }).join(''); }
  function renderCtl() {
    var h = '<div class="np-note info"><b>So wirkt die Steuerung:</b> Die Seite schreibt den Wert intern in das Objekt des Segments, wie ein empfangenes Telegramm (derselbe Weg wie gerätinterne Logik) – es geht kein Telegramm auf den Bus. Sperre, Startverzögerung und die Regeln der Effektkette gelten; die Status-Objekte senden ihre Änderungen wie gewohnt. ' + '<span class="np-f">POST /neo/control</span>' +
      '<br><b>Achtung:</b> Wirkt nur auf dieses Gerät; das nächste Telegramm über die Gruppenadresse überschreibt den Zustand; Logik und Visualisierung sehen die Änderung nur über die Status-Objekte; bei einem Slave mit „Lokal hat Vorrang“ löst sich das Gerät aus der Kette.' +
      (sc.segs.some(function (s) { return s.ch.m === 1; }) ? '<br><b>Ausnahme Effektkette-Master:</b> Ändert die Steuerung Ein/Aus, Effekt, Tempo, Farbe oder Helligkeit eines Master-Segments, sendet das Gerät wie immer ein Sync-Telegramm, und die Slaves folgen.' : '') +
      '<br><span class="np-f"><b>Angeboten wird, was für das Segment konfiguriert ist</b> – auch ohne Gruppenadresse: Ein/Aus, Helligkeit, Dimmen, Sperre und Effekt für jedes aktive Segment; Farbe nach Streifentyp (RGB, RGBW, RGB + WW/CW); Szenen 1 bis „Anzahl Szenen“; Effektmanager nur, wenn er in den Basiseinstellungen verfügbar, aktiv und mit Cues angelegt ist. 2D-Effekte nur bei 2D-Segmenten.</span></div>';
    sc.segs.forEach(function (s, i) {
      var lay8 = sc.bar.layout, lk = lookOf(s), c = rgbOf(s.c1), rows = '', ems = emsFor(), fxList = is2D(s.geo) ? FX1D.concat(FX2D) : FX1D;
      if (has(s, 'power')) rows += ctlRow('Ein/Aus', '<span class="np-seg2">' + cbtn(i, 'power', 1, 'Ein', (s.st === 'on' || s.st === 'lock') && s.bri > 0) + cbtn(i, 'power', 0, 'Aus', s.st === 'off') + '</span>');
      if (has(s, 'bri') || has(s, 'dim')) rows += ctlRow('Helligkeit',
        (has(s, 'bri') ? '<input type="range" class="np-rng" min="0" max="100" value="' + s.bri + '" data-seg="' + i + '" data-f="bri" aria-label="Helligkeit absolut"><b class="np-num" id="ctl-bri-' + i + '">' + s.bri + ' %</b>' : '<b class="np-num" id="ctl-bri-' + i + '">' + s.bri + ' %</b>') +
        (has(s, 'dim') ? '<span class="np-seg2">' + cbtn(i, 'dim', -1, '− dunkler', 0, ' data-hold="1"') + cbtn(i, 'dim', 1, '+ heller', 0, ' data-hold="1"') + '</span><span class="np-f">halten = dimmen (3 s für 0–100 %)</span>' : ''));
      if (has(s, 'rgb') || has(s, 'rgbw')) {
        var col = '<input type="color" class="np-col" value="' + s.c1 + '" data-seg="' + i + '" data-f="' + (has(s, 'rgbw') ? 'rgbw' : 'rgb') + '" aria-label="Farbe">';
        if (has(s, 'rgbw')) col += '<label class="np-wl">W <input type="range" class="np-rng sm" min="0" max="255" value="' + (s.w || 0) + '" data-seg="' + i + '" data-f="rgbw-w"></label>';
        if (has(s, 'cct')) col += '<label class="np-wl">WW <input type="range" class="np-rng sm" min="0" max="255" value="' + (s.w || 0) + '" data-seg="' + i + '" data-f="ww"></label><label class="np-wl">CW <input type="range" class="np-rng sm" min="0" max="255" value="' + (s.cw || 0) + '" data-seg="' + i + '" data-f="cw"></label>';
        rows += ctlRow('Farbe', col + '<span class="np-f">' + (has(s, 'rgbw') ? 'RGBW (Streifentyp RGBW)' : has(s, 'cct') ? 'RGB + WW/CW (Streifentyp RGBCCT)' : 'RGB (Streifentyp RGB)') + ' · ' + s.c1.toUpperCase() + (s.w ? ' + W ' + s.w : '') + (s.cw ? ' + CW ' + s.cw : '') + '</span>');
      }
      if (has(s, 'fx')) rows += ctlRow('Effekt', '<select class="np-sel" data-seg="' + i + '" data-f="fx">' + fxList.map(function (n) { return '<option' + (n === s.fx ? ' selected' : '') + '>' + n + '</option>'; }).join('') + '</select>' + (is2D(s.geo) ? '' : '<span class="np-f">2D-Effekte nur bei 2D-Segmenten</span>'));
      if (has(s, 'scene') && s.scenes && s.scenes.length) rows += ctlRow('Szene', s.scenes.map(function (q) { return cbtn(i, 'scene', q.n, 'Szene ' + q.n, s.scene === q.n); }).join(''));
      if (ems.length) rows += ctlRow('Effektmanager', ems.map(function (e) {
        var run = s.em === e.id && /run|pause/.test(s.ems || ''), act = e.st === 1;
        return '<div class="np-ctl-em"><span><b class="np-emlink" data-emgo="' + e.id + '" role="link" tabindex="0" title="Im Tab Effektmanager öffnen">EM ' + e.id + '</b>' + (run ? ' · Cue ' + s.cue + '/' + e.cues.length : '') + '</span>' +
          (act ? cbtn(i, 'emStart', e.id, '▶ Start', run) + (has(s, 'emStop') ? cbtn(i, 'emStop', 1, '■ Stop', 0) : '') + (has(s, 'cue') ? cbtn(i, 'cue', -1, 'Cue −', 0) + cbtn(i, 'cue', 1, 'Cue +', 0) : '')
            : '<span class="np-f">' + (e.st === 2 ? 'suspendiert: ein Start wird ignoriert' : 'deaktiviert') + '</span>') + '</div>';
      }).join(''));
      if (has(s, 'lock')) rows += ctlRow('Sperre', cbtn(i, 'lock', s.locked ? 0 : 1, s.locked ? 'Entsperren' : 'Sperren', s.locked));
      if (!rows) rows = '<div class="np-f">Für dieses Segment ist nichts einstellbar.</div>';
      var open = ctlIsOpen(i);
      h += '<div class="np-card np-ctl' + (open ? '' : ' closed') + '"><div class="np-card-h np-tog" data-tog="' + i + '" role="button" tabindex="0" aria-expanded="' + open + '"><span class="np-chev" aria-hidden="true">' + (open ? '▾' : '▸') + '</span><h3>' + badge(SEGT(i), i + 1) + segLabel(i) + '</h3>' + stPill(s) + '<span class="np-m np-num">' + s.bri + ' %</span>' + (emOn(s) ? '<span class="np-m">EM ' + s.em + ' · Cue ' + s.cue + '</span>' : '') + (s.ch.m ? '<span class="np-m">' + (s.ch.m === 1 ? 'Master' : 'Slave') + (s.ch.override ? ' · aus der Kette gelöst' : '') + '</span>' : '') + '<span class="np-sp"></span>' + swatch(lk.c1, lk.w, lk.cw) + '</div>' +
        (open ? '<canvas class="np-stage np-ctlcv" id="ctlcv-' + i + '" aria-label="LED-Bild des Segments"></canvas><div class="np-card-b">' + rows + '<div class="np-ctl-msg" id="ctl-msg-' + i + '"></div></div>' : '') + '</div>';
    });
    h += '<div class="np-ctl-log"><b>Letzte Befehle</b> <span class="np-f">(an dieses Gerät gesendet)</span><pre id="ctl-log">' + (ctlLog.length ? esc(ctlLog.join('\n')) : '—') + '</pre></div>';
    if (setH($('ctl'), h)) ctlSz = [];
  }
  var ctlSz = [];
  // Segment cards in Steuern fold; the first one starts open, the choice is kept across the 1 s refresh.
  var ctlOpen = {};
  function ctlIsOpen(i) { return ctlOpen[i] !== undefined ? ctlOpen[i] : i === 0; }
  function ctlToggle(i) { ctlOpen[i] = !ctlIsOpen(i); renderCtl(); }
  // "EM n" in Steuern: opens the tab Effektmanager with that EM unfolded.
  function goEm(id) {
    showTab('em');
    var c = qa('.np-em', $('em')).filter(function (x) { var b = x.querySelector('.np-em-h b'); return b && b.textContent === 'EM ' + id; })[0];
    if (!c) return;
    if (c.dataset.fk) FOLD[c.dataset.fk] = true;
    c.classList.remove('closed'); var h = c.querySelector('.np-em-h'); if (h) h.setAttribute('aria-expanded', 'true');
    c.scrollIntoView({ block: 'start', behavior: 'smooth' });
  }
  // Top view: a click on a segment opens its card in "Steuern"; areas are in plan units, plan.ox/oy/s map them to CSS px.
  function planSegAt(e) {
    var P = planMain, r = $('plan').getBoundingClientRect(), hit = -1;
    if (!P.G || TPL[lay.tpl].cube) return -1;
    var x = (e.clientX - r.left - P.ox) / P.s, y = (e.clientY - r.top - P.oy) / P.s;
    P.G.areas.forEach(function (a) { if (a.run && a.run.src === 'seg' && x >= a.x && x <= a.x + a.w && y >= a.y && y <= a.y + a.h) hit = a.run.seg; });
    return hit;
  }
  $('plan').addEventListener('click', function (e) {
    var i = planSegAt(e); if (i < 0) return;
    ctlOpen[i] = true; showTab('ctl'); renderCtl();
    var h = document.querySelector('#ctl [data-tog="' + i + '"]'); if (h) h.scrollIntoView({ block: 'start', behavior: 'smooth' });
  });
  $('plan').addEventListener('mousemove', function (e) { if (!TPL[lay.tpl].cube) this.style.cursor = planSegAt(e) >= 0 ? 'pointer' : 'default'; });
  function drawCtl(t) { // compact LED image per segment, same colours as the top view
    sc.segs.forEach(function (s, i) {
      var cv = $('ctlcv-' + i); if (!cv) return;
      var g = s.geo, two = is2D(g), z = ctlSz[i] || (ctlSz[i] = sizeCanvas(cv, two ? Math.min(90, 8 * g[1] + 12) : 30)), c = cv.getContext('2d'), n = s.b - s.a + 1, pad = 10;
      c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = STAGE; c.fillRect(0, 0, z.w, z.h);
      var W = two ? g[0] : Math.min(n, Math.floor((z.w - 2 * pad) / 4)), H = two ? g[1] : 1, cw = (z.w - 2 * pad) / W, ch = two ? (z.h - 12) / H : 10;
      for (var y = 0; y < H; y++) for (var x = 0; x < W; x++) {
        var li = two ? fwd(g[3], x, y, g[0], g[1]) : Math.floor(x * n / W), col = segPx(s, li, t, own(s)), m = col ? mix(OFF, col, clamp(col[3], 0, 1)) : OFF;
        var px = pad + x * cw, py = two ? 6 + y * ch : (z.h - ch) / 2;
        c.fillStyle = rgb(m); c.fillRect(px + .5, py + .5, Math.max(1, cw - 1), Math.max(1, ch - 1));
        if (!col || col[3] < .05) { c.strokeStyle = OFFE; c.lineWidth = .5; c.strokeRect(px + .5, py + .5, Math.max(1, cw - 1), Math.max(1, ch - 1)); }
        else if (pale(m)) { c.strokeStyle = 'rgba(150,115,60,.65)'; c.lineWidth = .8; c.strokeRect(px + .5, py + .5, Math.max(1, cw - 1), Math.max(1, ch - 1)); }
      }
    });
  }
  function ctlMsg(i, t) { var el = $('ctl-msg-' + i); if (el) el.textContent = t; }
  // One command to the device; its result arrives with a later /neo/state (cmds: [seq, result] of the last commands).
  function sendCtl(i, f, v) {
    var body = { segment: i + 1, function: f, value: v };
    ctlLog.unshift('POST /neo/control ' + JSON.stringify(body)); ctlLog.length = Math.min(ctlLog.length, 6);
    var lg = $('ctl-log'); if (lg) lg.textContent = ctlLog.join('\n');
    ctlMsg(i, 'Gesendet …');
    body.tok = LIVE.tok; // from /neo/state; the device refuses a command without it
    fetch('/neo/control', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) })
      .then(function (r) { return r.json().then(function (j) { return [r.status, j]; }); })
      .then(function (a) { if (a[0] === 200 && a[1].seq) { LIVE.pend[a[1].seq] = { i: i, f: f, v: v }; LIVE.lastSeq = a[1].seq; } else ctlMsg(i, a[0] === 503 ? 'Gerät beschäftigt, bitte noch einmal.' : (a[0] === 400 && a[1].error === 'bad command') ? 'Abgelehnt: ungültiger Befehl oder Gerät neu gestartet (neue Sitzung) – bitte noch einmal senden.' : 'Abgelehnt: ' + (a[1].error || a[0])); })
      .catch(function () { ctlMsg(i, 'Keine Verbindung zum Gerät.'); });
  }
  function liveHint(i, f, v) { // what the device does with a command beyond the KO semantics
    var s = sc.segs[i], t = '';
    if (!s) return t;
    if (!sc.bar.power && s.st !== 'lock') t += ' Das Gerät ist global aus: das Segment bleibt dunkel.';
    if (s.ch.m === 2 && f !== 'lock' && !(f === 'dim' && +v === 0)) t += s.ch.pol === 1 ? ' Slave mit „Lokal hat Vorrang“: übernimmt keine Sync-Telegramme mehr bis zum Neustart.' : ' Slave mit „Sync übernimmt immer“: das nächste Sync-Telegramm überschreibt das wieder.';
    return t;
  }
  $('ctl').addEventListener('click', function (e) {
    var g = e.target.closest('[data-emgo]'); if (g) { goEm(+g.dataset.emgo); return; }
    var t = e.target.closest('[data-tog]'); if (t && t.dataset.tog !== undefined) { ctlToggle(+t.dataset.tog); return; }
    var b = e.target.closest('button[data-f]'); if (!b) return;
    var i = +b.dataset.seg;
    if (b.dataset.hold) { if (e.detail === 0) { sendCtl(i, 'dim', +b.dataset.v); setTimeout(function () { sendCtl(i, 'dim', 0); }, 250); } return; } // keyboard: short step
    var f = b.dataset.f, v = b.dataset.v;
    if (f === 'fx') return;
    sendCtl(i, f, f === 'rgb' || f === 'rgbw' ? v : +v);
  });
  $('ctl').addEventListener('keydown', function (e) {
    var g = e.target.closest && e.target.closest('[data-emgo]');
    if (g && e.key === 'Enter') { e.preventDefault(); goEm(+g.dataset.emgo); return; }
    var t = e.target.closest && e.target.closest('[data-tog]');
    if (t && (e.key === 'Enter' || e.key === ' ')) { e.preventDefault(); ctlToggle(+t.dataset.tog); }
  });
  $('ctl').addEventListener('change', function (e) {
    var el = e.target, d = el.dataset; if (!d.f) return;
    var i = +d.seg, s = sc.segs[i];
    if (d.f === 'rgbw' || d.f === 'rgbw-w') { var lc = d.f === 'rgbw' ? el.value : s.c1, lw = d.f === 'rgbw-w' ? +el.value : s.w || 0; sendCtl(i, 'rgbw', lc + '/' + lw); return; } // one RGBW telegram: colour + W
    if (d.f === 'fx') { var id = FX1D.concat(FX2D).indexOf(el.value); if (id >= 0) sendCtl(i, 'fx', id); return; }
    sendCtl(i, d.f, d.f === 'rgb' ? el.value : +el.value);
  });
  $('ctl').addEventListener('input', function (e) { // live label while dragging; the value is applied on release
    var el = e.target; if (el.dataset.f === 'bri') { var b = $('ctl-bri-' + el.dataset.seg); if (b) b.textContent = el.value + ' %'; }
  });
  (function holdDim() { // DPT 3.007: press = start, release = stop telegram
    var liveSeg = -1;
    function stop() { ctlHeld = false; if (liveSeg >= 0) { var k = liveSeg; liveSeg = -1; sendCtl(k, 'dim', 0); } }
    $('ctl').addEventListener('pointerdown', function (e) {
      var b = e.target.closest('button[data-hold]'); if (!b) return;
      e.preventDefault();
      if (liveSeg >= 0) return;
      liveSeg = +b.dataset.seg; ctlHeld = true; sendCtl(liveSeg, 'dim', +b.dataset.v);
    });
    ['pointerup', 'pointercancel', 'pointerleave'].forEach(function (ev) { $('ctl').addEventListener(ev, function (e) { if (e.target.closest && e.target.closest('button[data-hold]')) stop(); }); });
    // window: release and cancel bubble up from any element, also after the pointer left the button
    window.addEventListener('pointerup', stop); window.addEventListener('pointercancel', stop); window.addEventListener('blur', stop);
  })();

  // ---- arrangement editor -------------------------------------------------------------------------
  function srcVal(r) { return r.src === 'seg' ? 'seg:' + r.seg : r.src === 'strip' ? 'strip:' + r.strip : r.src === 'chain' ? 'chain:' + r.base : 'none'; }
  function srcOptions(r) {
    var o = [['none', '— nicht belegt']];
    sc.segs.forEach(function (s, i) { o.push(['seg:' + i, 'Segment ' + (i + 1) + ' (LED ' + (s.a + 1) + '–' + (s.b + 1) + ')']); });
    sc.strips.forEach(function (s, i) { o.push(['strip:' + i, 'Streifen S' + (i + 1) + ' (' + s.leds + ' LEDs)']); });
    if (sc.chain) sc.chain.nodes.forEach(function (n) { if (!n.me) o.push(['chain:' + n.off, 'Kette ' + n.off + '–' + (n.off + n.len - 1) + ' · anderes Gerät']); });
    var v = srcVal(r);
    return o.map(function (x) { return '<option value="' + x[0] + '"' + (x[0] === v ? ' selected' : '') + '>' + x[1] + '</option>'; }).join('');
  }
  // Arrangement file: the same JSON is kept in localStorage and written/read by Export/Import.
  function layObj() { return { version: 1, vorlage: lay.tpl, masse: lay.p, kanten: lay.edge || null, teile: lay.runs.map(function (r) { var o = { name: r.name, quelle: srcVal(r), led0: r.rev ? 'Ende' : 'Anfang' }; if (TPL[lay.tpl].walls) o.aktiv = r.on !== 0; return o; }) }; }
  function layJson() { return JSON.stringify(layObj(), null, 1); }
  function layFrom(o) { // validated copy of an arrangement file, or null
    if (!o || typeof o !== 'object' || o.version !== 1 || typeof o.vorlage !== 'string' || !Object.prototype.hasOwnProperty.call(TPL, o.vorlage) || !o.masse || typeof o.masse !== 'object' || !Array.isArray(o.teile) || !o.teile.length) return null;
    var T = TPL[o.vorlage], maxRuns = T.free ? 6 : T.walls ? 4 : 8;
    if (o.teile.length > maxRuns || o.teile.some(function (t) { return !t || typeof t !== 'object' || Array.isArray(t); })) return null;
    var L = { tpl: o.vorlage, p: Object.assign({}, DEF[o.vorlage]), edge: Array.isArray(o.kanten) && o.kanten.length === 2 ? [String(o.kanten[0]), String(o.kanten[1])] : null, runs: [] };
    T.prm.forEach(function (q) { var v = parseInt(o.masse[q[0]], 10); if (!isNaN(v) && v >= q[2] && v <= q[3]) L.p[q[0]] = v; });
    o.teile.forEach(function (t, i) {
      var r = { name: String(typeof t.name === 'string' && t.name ? t.name : T.names ? T.names[i] : T.part + ' ' + (i + 1)).slice(0, 40), src: 'none', rev: t.led0 === 'Ende' ? 1 : 0 }, m = /^(seg|strip|chain):(\d{1,5})$/.exec(String(t.quelle || '')), n = m ? parseInt(m[2], 10) : -1;
      if (m && m[1] === 'seg' && sc.segs[n]) { r.src = 'seg'; r.seg = n; }
      else if (m && m[1] === 'strip' && sc.strips[n]) { r.src = 'strip'; r.strip = n; }
      else if (m && m[1] === 'chain' && sc.chain && sc.chain.nodes.some(function (x) { return x.off === n && !x.me; })) { r.src = 'chain'; r.base = n; }
      if (T.walls) r.on = t.aktiv === false || t.aktiv === 0 || t.aktiv === 'nein' ? 0 : 1;
      L.runs.push(r);
    });
    var keep = lay; lay = L; try { syncRuns(); } finally { lay = keep; }
    return L;
  }
  var LSK = 'openknx-neopixel-anordnung-v1:', LSID = 'live'; // suffix unchanged: arrangements saved by earlier versions stay valid
  function lsLoad(k) { try { var v = localStorage.getItem(LSK + k); return v ? layFrom(JSON.parse(v)) : null; } catch (e) { return null; } }
  function lsSave(k) { try { localStorage.setItem(LSK + k, layJson()); return true; } catch (e) { return false; } }
  function lsDrop(k) { try { localStorage.removeItem(LSK + k); } catch (e) { /* storage blocked: nothing stored */ } }
  function edMsg(t) { var el = $('ed-msg'); if (el) el.textContent = t; }
  function edState() { return lay.saved ? pill('ok', 'in diesem Browser gespeichert') : pill('idle', lsHas(LSID) ? 'geändert, nicht gespeichert' : 'nicht gespeichert'); }
  function lsHas(k) { try { return !!localStorage.getItem(LSK + k); } catch (e) { return false; } }
  function dirty() { lay.saved = 0; var el = $('ed-state'); if (el) el.innerHTML = edState(); renderCap(); }
  function renderEditor() {
    var T = TPL[lay.tpl], h = '<div class="np-card-h"><h3>Anordnung festlegen</h3><span id="ed-state">' + edState() + '</span><span class="np-sp"></span><button class="np-btn" data-act="close">Schließen</button></div><div class="np-ed-grid"><div class="np-ed-l"><div class="np-f">1 · Vorlage</div><div class="np-tpls">';
    Object.keys(TPL).forEach(function (k) { h += '<button class="np-tpl' + (k === lay.tpl ? ' on' : '') + '" data-tpl="' + k + '"><svg viewBox="0 0 48 28" fill="none" stroke="' + MUTE + '" stroke-width="2" stroke-linecap="round">' + TPL[k].icon + '</svg>' + TPL[k].name + '</button>'; });
    h += '</div><div class="np-f">2 · Maße</div>';
    if (T.free) h += '<label>Abschnitte<input type="number" min="1" max="6" data-runs="1" value="' + lay.runs.length + '"></label>';
    T.prm.forEach(function (p) { h += '<label>' + p[1] + '<input type="number" min="' + p[2] + '" max="' + p[3] + '" data-p="' + p[0] + '" value="' + lay.p[p[0]] + '"></label>'; });
    h += '</div><div class="np-ed-r"><div class="np-f">LED-Bild, live</div><canvas class="np-stage np-edcv" id="edplan"></canvas>' +
      '<div class="np-f">3 · Zuordnung: was liegt auf welchem Teil?' + (T.walls ? ' Wände in Einfahrtsrichtung; jede Wand einzeln an- oder abwählen.' : '') + '</div><div class="np-tw"><table class="np-map"><tr>' + (T.walls ? '<th>Wand</th>' : '') + '<th>Teil</th><th>Quelle</th><th>LED 0</th></tr>';
    lay.runs.forEach(function (r, i) {
      var off = T.walls && r.on === 0;
      h += '<tr' + (off ? ' class="np-off"' : '') + '>' + (T.walls ? '<td><label class="np-chk"><input type="checkbox" data-on="' + i + '"' + (off ? '' : ' checked') + '>' + T.names[i] + '</label></td>' : '') +
        '<td><input data-name="' + i + '" value="' + esc(r.name) + '"' + (off ? ' disabled' : '') + ' style="width:120px;padding:3px 6px;margin:0;border:1px solid ' + LINE + ';border-radius:4px"></td><td><select data-src="' + i + '"' + (off ? ' disabled' : '') + '>' + srcOptions(r) + '</select></td>' +
        '<td><select data-rev="' + i + '"' + (off ? ' disabled' : '') + '><option value="0"' + (r.rev ? '' : ' selected') + '>am Anfang</option><option value="1"' + (r.rev ? ' selected' : '') + '>am Ende</option></select></td></tr>';
    });
    h += '</table></div><div class="np-note info">Nur eine Anzeigehilfe: Sie ändert nichts an der LED-Ausgabe; welche LED was zeigt, bestimmen weiterhin Segmente, Geometrie, Verdrahtung und Kette aus der Gerätekonfiguration. ' +
      '<b>Gespeichert wird in diesem Browser.</b> Mit Exportieren und Importieren (JSON-Datei) nimmst du die Anordnung in einen anderen Browser oder aufs Handy mit. Später möglich: Speichern auf dem Gerät (/neo/layout.json).</div></div></div>' +
      '<div class="np-ed-f"><button class="np-btn pri" data-act="save">Im Browser speichern</button><button class="np-btn" data-act="export">Exportieren</button><button class="np-btn" data-act="import">Importieren</button><input type="file" id="ed-file" accept=".json,application/json" hidden><button class="np-btn" data-act="reset">Zurücksetzen</button>' + (lsHas(LSID) ? '<button class="np-btn" data-act="drop">Gespeicherte entfernen</button>' : '') + '<span class="np-f" id="ed-msg">Änderungen siehst du sofort im LED-Bild und in der Draufsicht.</span><details><summary>Inhalt der Datei</summary><pre id="ed-json"></pre></details></div>';
    $('editor').innerHTML = h; $('ed-json').textContent = layJson();
  }
  function relayout() { layoutPlan(); renderCap(); if ($('ed-json')) $('ed-json').textContent = layJson(); }
  $('editor').addEventListener('click', function (e) {
    var b = e.target.closest('button'); if (!b) return;
    if (b.dataset.tpl) { lay.stash = lay.stash || {}; lay.stash[lay.tpl] = clone(lay.runs); lay.pstash = lay.pstash || {}; lay.pstash[lay.tpl] = clone(lay.p); lay.tpl = b.dataset.tpl; if (lay.stash[lay.tpl]) lay.runs = clone(lay.stash[lay.tpl]); lay.saved = 0; lay.p = Object.assign({}, DEF[lay.tpl], lay.tpl === sc.layout.tpl ? sc.layout.p : {}, lay.pstash[lay.tpl] || {}); lay.edge = lay.tpl === sc.layout.tpl ? sc.layout.edge : null; syncRuns(); renderEditor(); relayout(); }
    if (b.dataset.act === 'close') { $('editor').hidden = true; $('lay-edit').classList.remove('pri'); }
    if (b.dataset.act === 'reset') { lay = clone(sc.layout); renderEditor(); relayout(); edMsg('Standard-Anordnung geladen; eine gespeicherte Anordnung bleibt, bis du speicherst oder sie entfernst.'); }
    if (b.dataset.act === 'drop') { var had = lsHas(LSID); lsDrop(LSID); var gone = !lsHas(LSID); if (lay.saved && gone) lay.saved = 0; renderEditor(); relayout(); edMsg(!had ? 'In diesem Browser ist keine Anordnung gespeichert.' : gone ? 'Gespeicherte Anordnung in diesem Browser entfernt.' : 'Entfernen nicht möglich: der Browser erlaubt hier keinen Zugriff auf den lokalen Speicher.'); }
    if (b.dataset.act === 'save') { if (lsSave(LSID)) { lay.saved = 1; renderEditor(); relayout(); edMsg('In diesem Browser gespeichert.'); } else edMsg('Speichern nicht möglich: der Browser erlaubt hier keinen lokalen Speicher. Exportieren geht trotzdem.'); }
    if (b.dataset.act === 'export') {
      try { var u = URL.createObjectURL(new Blob([layJson()], { type: 'application/json' })), a = document.createElement('a'); a.href = u; a.download = 'neopixel-anordnung.json'; document.body.appendChild(a); a.click(); a.remove(); setTimeout(function () { URL.revokeObjectURL(u); }, 1000); edMsg('Datei neopixel-anordnung.json erstellt.'); }
      catch (err) { edMsg('Export nicht möglich: ' + err.message); }
    }
    if (b.dataset.act === 'import') $('ed-file').click();
  });
  $('editor').addEventListener('input', function (e) {
    var el = e.target, d = el.dataset, v = parseInt(el.value, 10);
    if (d.name) dirty();
    if (d.runs) { if (isNaN(v) || v < 1 || v > 6) return; dirty(); while (lay.runs.length < v) lay.runs.push({ name: TPL[lay.tpl].part + ' ' + (lay.runs.length + 1), src: 'none', rev: 0 }); lay.runs.length = v; renderEditor(); relayout(); return; }
    if (d.p) { var p = TPL[lay.tpl].prm.filter(function (x) { return x[0] === d.p; })[0]; if (isNaN(v) || v < p[2] || v > p[3]) return; dirty(); lay.p[d.p] = v; var n = lay.runs.length; syncRuns(); if (n !== lay.runs.length) renderEditor(); relayout(); }
    if (d.name) { lay.runs[+d.name].name = el.value; relayout(); }
  });
  $('editor').addEventListener('change', function (e) {
    var d = e.target.dataset, v = e.target.value;
    if (d.p) { e.target.value = lay.p[d.p]; return; } // out-of-range input snaps back to the value in use
    if (d.runs) { e.target.value = lay.runs.length; return; }
    if (d.src || d.rev || d.on) dirty();
    if (d.src) { var r = lay.runs[+d.src], q = v.split(':'); r.src = q[0]; delete r.seg; delete r.strip; delete r.base; if (q[0] === 'seg') r.seg = +q[1]; if (q[0] === 'strip') r.strip = +q[1]; if (q[0] === 'chain') r.base = +q[1]; relayout(); }
    if (d.rev) { lay.runs[+d.rev].rev = +v; relayout(); }
    if (d.on) { lay.runs[+d.on].on = e.target.checked ? 1 : 0; renderEditor(); relayout(); }
    if (e.target.id === 'ed-file' && e.target.files && e.target.files[0]) {
      var fr = new FileReader();
      fr.onload = function () { var L = null; try { L = layFrom(JSON.parse(fr.result)); } catch (err) { L = null; } if (!L) { edMsg('Import abgelehnt: keine gültige Anordnungsdatei.'); return; } L.saved = 0; lay = L; renderEditor(); relayout(); edMsg('Importiert. Mit „Im Browser speichern“ übernehmen.'); };
      fr.readAsText(e.target.files[0]);
    }
  });
  $('lay-edit').addEventListener('click', function () { var ed = $('editor'); ed.hidden = !ed.hidden; this.classList.toggle('pri', !ed.hidden); if (!ed.hidden) { layoutPlan(); ed.scrollIntoView({ block: 'nearest', behavior: 'smooth' }); } });

  // ---- tab switching ------------------------------------------------------------------------------
  function setHash() { try { history.replaceState(null, '', '#t-' + tab); } catch (e) { /* file:// in some browsers */ } }
  function renderMode() {
    applyUnconf();
    $('meta').innerHTML = LIVE.err ? pill(LIVE.ok ? 'warn' : 'bad', LIVE.err) : ''; // only connection problems; the interval is shown at the top right
  }
  // Replace a panel only when its markup changed: the 1 s poll otherwise rebuilt every panel and disturbed scrolling and selection.
  function setH(el, h) { if (!el || el._h === h) return false; el._h = h; el.innerHTML = h; foldApply(el); return true; }
  // Every card folds on a click on its head; per panel the first card starts open, the choice survives the refresh.
  // Steuern keeps its own folding (closed cards render no controls).
  var FOLD = {};
  function foldApply(root) {
    if (!root || !root.querySelectorAll) return;
    var base = root.id || 'neo', n = 0;
    qa('.np-card, .np-em', root).forEach(function (c) {
      if (c.classList.contains('np-ctl')) return;
      var h = c.querySelector('.np-card-h, .np-em-h'); if (!h || h.parentNode !== c) return;
      var k = base + ':' + (n++); c.dataset.fk = k; h.classList.add('np-fold');
      var open = FOLD[k] !== undefined ? FOLD[k] : n === 1;
      c.classList.toggle('closed', !open); h.setAttribute('aria-expanded', open ? 'true' : 'false');
    });
  }
  ROOT.addEventListener('click', function (e) {
    var h = e.target.closest && e.target.closest('.np-fold'); if (!h || e.target.closest('button, input, select, a, label, canvas')) return;
    var c = h.parentNode; if (!c || !c.dataset || !c.dataset.fk) return;
    FOLD[c.dataset.fk] = c.classList.contains('closed'); c.classList.toggle('closed'); h.setAttribute('aria-expanded', FOLD[c.dataset.fk] ? 'true' : 'false');
    if (FOLD[c.dataset.fk]) { ctlSz = []; if (tab === 'setup') layoutPlan(); if (tab === 'chain') layoutChain(); }
  });
  function render() {
    T0 = performance.now(); lay = lsLoad(LSID); if (lay) lay.saved = 1; else lay = clone(sc.layout); plan.cube = { yaw: .6, pitch: .45, zoom: 1, auto: true };
    power = sc.power;
    renderMode();
    renderBar(); renderPipe(); renderSegs(); renderEm(); renderChain(); renderStrips(); renderHw(); renderEditor(); renderCtl();
    showTab(tab);
  }
  function showTab(t) {
    tab = t;
    qa('.np-tab').forEach(function (x) { x.classList.toggle('active', x.dataset.t === t); });
    qa('.np-pane').forEach(function (p) { p.hidden = p.id !== 'p-' + t; });
    if (t === 'setup') { layoutPlan(); renderCap(); }
    if (t === 'chain') layoutChain();
    if (t === 'ctl') ctlSz = [];
    applyUnconf();
    setHash();
  }
  // Unconfigured device (no configuration loaded): only the hint, no status bar, tabs or panels.
  function applyUnconf() {
    var u = !!LIVE.unconf, U = LIVE.unconf;
    $('unconf').hidden = !u; $('bar').hidden = u; $('bar').parentNode.hidden = u; $('tabs').hidden = u; // parent: band with refresh controls
    qa('.np-pane').forEach(function (p) { p.hidden = u || p.id !== 'p-' + tab; });
    if (u) $('unconf').innerHTML = '<b>Gerät nicht konfiguriert – bitte Gerätekonfiguration laden.</b><br>Ohne geladene Konfiguration sind keine Streifen, Segmente und Effekte angelegt; diese Seite zeigt den Status, sobald die Applikation geladen ist.' +
      '<br><span class="np-f">' + (U.board ? esc(U.board) + ' · ' : '') + 'HW-ID ' + hex4(U.hwid || 0) + ' · Laufzeit ' + upStr(U.up || 0) + '</span>';
  }
  $('tabs').addEventListener('click', function (e) { var b = e.target.closest('.np-tab'); if (b) showTab(b.dataset.t); });
  window.addEventListener('resize', function () { ctlSz = []; if (tab === 'setup') layoutPlan(); if (tab === 'chain') layoutChain(); });

  var h0 = location.hash.replace('#', '').replace(/^t-/, '').split(':')[0]; // '#t-hw'; '#hw' collided with the panel ids and made the browser jump // '#setup', also the older '#setup:live'
  if (h0 && $('p-' + h0)) tab = h0;
  // Refresh interval chosen at the top right, kept in this browser; 'aus' stops polling until the next manual refresh.
  var refresh = 1, pollT = 0;
  try { var rfs = localStorage.getItem('openknx-neopixel-refresh'); if (rfs !== null && [0, 1, 2, 5, 10, 30, 60].indexOf(+rfs) >= 0) refresh = +rfs; } catch (e) { /* storage blocked */ }
  function pollNow() { clearTimeout(pollT); pollT = 0; livePoll(); }
  $('rf-sel').value = String(refresh);
  $('rf-sel').addEventListener('change', function () {
    refresh = +this.value; try { localStorage.setItem('openknx-neopixel-refresh', String(refresh)); } catch (e) { /* storage blocked */ }
    renderMode(); pollNow();
  });
  $('rf-now').addEventListener('click', pollNow);
  sc = fromState(EMPTY, []);
  render();
  qa('.np-pane').forEach(foldApply);
  // Loading overlay (base.css) until the first snapshot; until then a failed request is repeated even with polling off.
  var ld = document.createElement('div'); ld.className = 'ld'; ld.textContent = 'Lade Daten …';
  (document.querySelector('main') || document.body).appendChild(ld);
  livePoll();
  pixPoll();

  // ---- live polling ---------------------------------------------------------------------------------
  function liveSig(L) { return [L.segs.length, L.strips.length, L.bar.layout, L.emCount, L.chain ? L.chain.seg : -1, L.segs.map(function (s) { return s.a + '-' + s.b + ':' + s.geo.join('x'); }).join(',')].join('|'); }
  var ctlHtml = '';
  function onState(j) {
    if (ld) { ld.remove(); ld = null; }
    LIVE.rxAt = performance.now(); LIVE.ok = 1; LIVE.err = '';
    if (j.unconf) { // no configuration loaded: hint only, nothing to show or control
      var was = LIVE.unconf; LIVE.unconf = j; LIVE.st = null; LIVE.sig = ''; LIVE.pend = {};
      if (!was) render(); else renderMode();
      return;
    }
    if (LIVE.unconf) { LIVE.unconf = null; LIVE.sig = ''; }
    if (LIVE.st && j.up < LIVE.st.up) { Object.keys(LIVE.pend).forEach(function (k) { ctlMsg(LIVE.pend[k].i, 'Gerät wurde neu gestartet, Ergebnis unbekannt.'); }); LIVE.pend = {}; } // device restarted: its command numbers start again
    LIVE.st = j; LIVE.tok = j.tok || '';
    if (j.emSum !== LIVE.emSum && !LIVE.emBusy) fetchEm();
    var L = fromState(j, LIVE.ems), sig = liveSig(L);
    sc = L; power = L.power;
    // command results: each command its own; one that dropped out of the device's list is unknown
    var res = {}, maxSeq = 0;
    (j.cmds || []).forEach(function (c) { res[c[0]] = c[1]; if (c[0] > maxSeq) maxSeq = c[0]; });
    var done = Object.keys(LIVE.pend).filter(function (k) { return +k <= maxSeq; });
    if (sig !== LIVE.sig) { LIVE.sig = sig; ctlHtml = ''; render(); } // configuration changed: full render, arrangement re-validated
    else {
      renderMode(); renderBar(); renderPipe(); renderSegs(); renderEm(); renderChain(); renderStrips(); renderHw();
      // a rebuilt canvas is blank until the next animation frame: draw it in the same task, or it flickers once a second
      if (tab === 'chain') { layoutChain(); if (sc.chain) drawChain((performance.now() - T0) / 1000); }
      if (tab === 'setup') renderCap();
      var foc = document.activeElement, busy = foc && $('ctl').contains(foc) && /INPUT|SELECT/.test(foc.tagName);
      if (!busy && !ctlHeld) { var keep = qa('.np-ctl-msg').map(function (m) { return [m.id, m.textContent]; }); renderCtl(); keep.forEach(function (m) { var el = $(m[0]); if (el) el.textContent = m[1]; }); if (tab === 'ctl') drawCtl((performance.now() - T0) / 1000); }
    }
    done.forEach(function (k) {
      var p = LIVE.pend[k], r = res[k]; delete LIVE.pend[k];
      ctlMsg(p.i, r === undefined ? 'Ergebnis unbekannt (zu viele Befehle in kurzer Zeit).' : (RESN[r] || 'Ergebnis ' + r) + (r === 0 || r === 2 ? liveHint(p.i, p.f, p.v) : ''));
    });
  }
  function fetchEm() {
    LIVE.emBusy = 1;
    fetch('/neo/em', { cache: 'no-store' }).then(function (r) { return r.json(); }).then(function (e) {
      LIVE.emBusy = 0;
      if (!e || e.pending || !e.ems) return; // not built yet: the next snapshot asks again
      LIVE.ems = e.ems; LIVE.emSum = e.sum;
    }).catch(function () { LIVE.emBusy = 0; });
  }
  function livePoll() {
    var wait = refresh ? (document.hidden ? Math.max(5000, refresh * 1000) : refresh * 1000) : 0;
    fetch('/neo/state', { cache: 'no-store' }).then(function (r) {
      if (r.status === 503) return { busy: 1 };
      if (!r.ok) throw new Error('HTTP ' + r.status);
      return r.json();
    }).then(function (j) {
      if (j.busy) { wait = 300; LIVE.err = 'Gerät beschäftigt, erneut versuchen …'; renderMode(); return; }
      if (j.pending) { wait = 300; return; }
      onState(j);
    }).catch(function () { LIVE.ok = 0; LIVE.err = 'keine Verbindung'; renderMode(); if (ld) { wait = 1500; ld.textContent = 'Keine Verbindung, neuer Versuch …'; } }).then(function () { if (wait) pollT = setTimeout(livePoll, wait); });
  }
  function pixPoll() { // LED image while a tab with LEDs is open
    if (LIVE.unconf || document.hidden || !refresh || !/setup|ctl|chain/.test(tab)) { setTimeout(pixPoll, 500); return; }
    fetch('/neo/pixels', { cache: 'no-store' }).then(function (r) { return r.ok ? r.arrayBuffer() : null; }).then(function (b) {
      var d = b && new Uint8Array(b);
      if (d && d.length >= 8 && d[0] === 78 && d[1] === 80 && d[2] === 1) { var n = d[4] | d[5] << 8, bpl = d[3]; if (d.length >= 8 + n * bpl) LIVE.px = { d: d, n: n, bpl: bpl }; }
    }).catch(function () { /* next try below */ }).then(function () { setTimeout(pixPoll, 200); });
  }

  // ---- animation ----------------------------------------------------------------------------------
  function frame(now) {
    requestAnimationFrame(frame);
    if (now - last < 32) return;
    last = now;
    var t = (now - T0) / 1000;
    if (tab === 'setup') drawPlan(t);
    else if (tab === 'chain' && sc.chain) drawChain(t);
    else if (tab === 'ctl') drawCtl(t);
  }
  requestAnimationFrame(frame);
})();
