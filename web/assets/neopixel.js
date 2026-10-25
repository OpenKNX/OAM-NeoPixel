// neopixel.js - NeoPixel status page /neo. "Live" renders this device from /neo/state, /neo/em and /neo/pixels;
// "Vorschau" renders example setups from doc/examples and a few special cases, always marked as example data.
(function () {
  'use strict';
  var ROOT = document.getElementById('neo');
  if (!ROOT) return;
  ROOT.innerHTML = "<div class=\"np-head\">\n  <div><h1>NeoPixel</h1><div class=\"meta\" id=\"meta\"></div></div>\n  <div class=\"np-mode\" id=\"mode\" role=\"group\" aria-label=\"Datenquelle\">\n    <button data-m=\"live\" class=\"on\" aria-pressed=\"true\"><span class=\"np-livedot\"></span>Live</button>\n    <button data-m=\"pv\" aria-pressed=\"false\">Vorschau</button>\n  </div>\n</div>\n<div class=\"np-mocknote\" id=\"mocknote\"></div>\n<div class=\"np-pv\" id=\"pv\" hidden>\n  <div class=\"np-pv-h\"><span class=\"np-pv-tag\">Vorschau</span><b>Beispieldaten, nicht dieses Ger\u00e4t</b><span class=\"np-sp\"></span><button class=\"np-btn\" id=\"pv-back\">Zur\u00fcck zu Live</button></div>\n  <div class=\"np-pv-sel\"><span class=\"np-f\">Vorlagen (doc/examples)</span><span id=\"pv-tpl\"></span></div>\n  <div class=\"np-pv-sel\"><span class=\"np-f\">Weitere F\u00e4lle</span><span id=\"pv-more\"></span></div>\n  <div class=\"np-pv-d\" id=\"pv-d\"></div>\n</div>\n<div class=\"np-bar\" id=\"bar\"></div>\n<div class=\"np-note bad\" id=\"unconf\" hidden></div>\n<div class=\"np-tabs\" id=\"tabs\">\n  <button class=\"np-tab active\" data-t=\"hw\">Hardware</button>\n  <button class=\"np-tab\" data-t=\"setup\">Aufbau</button>\n  <button class=\"np-tab\" data-t=\"ctl\">Steuern</button>\n  <button class=\"np-tab\" data-t=\"segs\">Segmente</button>\n  <button class=\"np-tab\" data-t=\"em\">Effektmanager</button>\n  <button class=\"np-tab\" data-t=\"chain\">Effektkette</button>\n  <button class=\"np-tab\" data-t=\"strips\">Streifen, Strom &amp; Relais</button>\n</div>\n<div class=\"np-pvline\" id=\"pvline\" hidden></div>\n\n<section class=\"np-pane\" id=\"p-hw\"><div id=\"hw\"></div></section>\n<section class=\"np-pane\" id=\"p-setup\" hidden>\n  <div class=\"np-card\">\n    <div class=\"np-card-h\"><h3>Anordnung \u00b7 Draufsicht mit LED-Bild</h3><span id=\"lay-name\" class=\"np-pill idle\"></span><span class=\"np-sp\"></span><button class=\"np-btn\" id=\"lay-edit\">Anordnung festlegen</button></div>\n    <canvas class=\"np-stage\" id=\"plan\"></canvas>\n    <div class=\"np-cap\" id=\"plan-cap\"></div>\n  </div>\n  <div class=\"np-editor\" id=\"editor\" hidden></div>\n  <div class=\"np-note info\"><b>Die Anordnung legst du fest.</b> Die Firmware wei\u00df nicht, wo die LEDs liegen: Sie kennt nur die Reihenfolge der Streifen, die Segmente, die 2D/3D-Geometrie mit Verdrahtung und den Versatz in der Effektkette. Die Anordnung ist eine Anzeigehilfe und \u00e4ndert nichts an der LED-Ausgabe. Sie wird in diesem Browser gespeichert und l\u00e4sst sich als JSON-Datei exportieren und in einem anderen Browser importieren; Speichern auf dem Ger\u00e4t (/neo/layout.json) ist eine sp\u00e4tere Option.</div>\n  <h2>Vom Streifen zum Segment</h2>\n  <div class=\"np-pipe\" id=\"pipe\"></div>\n  <div class=\"np-note\">LED-Bild = Inhalt des virtuellen Streifens, ohne HCL, Gamma und Strombegrenzung. Ausgeschaltete LEDs erscheinen hellgrau.</div>\n</section>\n<section class=\"np-pane\" id=\"p-ctl\" hidden><div id=\"ctl\"></div></section>\n<section class=\"np-pane\" id=\"p-segs\" hidden><div class=\"np-tw\" id=\"segs\"></div></section>\n<section class=\"np-pane\" id=\"p-em\" hidden><div id=\"em\"></div></section>\n<section class=\"np-pane\" id=\"p-chain\" hidden><div id=\"chain\"></div></section>\n<section class=\"np-pane\" id=\"p-strips\" hidden><div id=\"strips\"></div></section>\n<div class=\"np-foot\" id=\"foot\"></div>";
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
  function hsl(h, s, l) {
    h = (((h % 360) + 360) % 360) / 360;
    var q = l < .5 ? l * (1 + s) : l + s - l * s, p = 2 * l - q;
    function f(t) { t = (t + 1) % 1; return 255 * (t < 1 / 6 ? p + (q - p) * 6 * t : t < .5 ? q : t < 2 / 3 ? p + (q - p) * (2 / 3 - t) * 6 : p); }
    return [f(h + 1 / 3), f(h), f(h - 1 / 3)];
  }
  function hash01(a, b) { var x = Math.sin(a * 127.1 + b * 311.7) * 43758.5453; return x - Math.floor(x); }
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

  // ---- effects: rough look-alikes of the firmware effects; [r,g,b,intensity] or null = dark --------
  // p: i/n = index/length in the band, X/Y/W/H = position in the (global) matrix, t = seconds
  function U(p) { return p.W > 1 ? p.X / (p.W - 1) : 0; }
  function V(p) { return p.H > 1 ? p.Y / (p.H - 1) : 0; }
  var FX = {
    'Cylon': function (s, p) { // eye across the whole (global) matrix width
      var ph = (p.t % 6) / 6, pos = (ph < .5 ? ph * 2 : 2 - ph * 2) * (p.W - 1), a = clamp(1 - Math.abs(p.X - pos) / 2.6, 0, 1);
      return a > .03 ? rgbOf(s.c1).concat(a) : null;
    },
    'Noise': function (s, p) { var v = .5 + .25 * Math.sin(U(p) * 9 + p.t * .5) + .25 * Math.sin(V(p) * 4 - p.t * .7 + U(p) * 5); return hsl(8 + 38 * v, .95, .55).concat(.35 + .65 * v); },
    'Plasma Nebula': function (s, p) {
      var u = U(p) * 6, v = V(p) * 6, w = Math.sin(u + p.t * .7) + Math.sin(v * .8 - p.t * .5) + Math.sin((u + v) * .6 + p.t * .3) + Math.sin(Math.sqrt((u - 3) * (u - 3) + (v - 3) * (v - 3)) * 1.3 - p.t);
      w = .5 + w / 8; return hsl(250 + 110 * w, .85, .55).concat(.3 + .7 * w);
    },
    'Rainbow': function (s, p) { return hsl(p.i / p.n * 360 + p.t * 40, .9, .55).concat(1); },
    'Juggle': function (s, p) {
      var best = null, a = 0;
      for (var j = 0; j < 4; j++) { var pos = (Math.sin(p.t * (.6 + j * .17) + j) * .5 + .5) * (p.n - 1), q = clamp(1 - Math.abs(p.i - pos) / 10, 0, 1); if (q > a) { a = q; best = hsl(j * 90 + 20, .9, .55); } }
      return a > .03 ? best.concat(a) : null;
    },
    'Sparkle': function (s, p) { return hash01(p.i, Math.floor(p.t * 6)) < .05 ? [255, 246, 226, 1] : null; },
    'Breathing': function (s, p) { return rgbOf(s.c1).concat(.2 + .8 * (.5 + .5 * Math.sin(p.t * 1.5))); },
    'Pride2015': function (s, p) { return hsl(p.i * 2 + p.t * 40, .85, .55).concat(.4 + .6 * (.5 + .5 * Math.sin(p.i * .15 + p.t * 2))); },
    'Strobe': function (s, p) { return Math.floor(p.t * 8) % 4 === 0 ? rgbOf(s.c1).concat(1) : null; },
    'Kerze': function (s, p) { return rgbOf(s.c1).concat(.55 + .45 * hash01(p.i >> 3, Math.floor(p.t * 10))); },
    'Gradient': function (s, p) { var v = ((U(p) + p.t * .02) % 1 + 1) % 1, h = 45 * (.5 + .5 * Math.cos(v * 6.283)); return hsl(h, .95, .5 + .06 * Math.sin(v * 12.5)).concat(.85); },
    'Aurora 2D': function (s, p) {
      var u = U(p), v = V(p), c = Math.sin(u * 11 + p.t * .7 + 1.7 * Math.sin(v * 2.4 + p.t * .35)) * .5 + .5;
      c = Math.pow(c, 1.6) * (.55 + .45 * Math.sin(u * 3.1 - p.t * .2 + v));
      return c > .04 ? hsl(18 + 46 * (1 - v) + 20 * Math.sin(u * 4 + p.t * .3), .9, .55).concat(clamp(.2 + c, 0, 1)) : null;
    },
    'Fire': function (s, p) {
      var rows = p.H > 1, h = rows ? 1 - V(p) * .85 : .7, n = hash01(Math.floor(rows ? p.X : p.i / 3), Math.floor(p.t * 9 + (rows ? p.Y : 0)));
      h = clamp(h * (.55 + .45 * n) + .12 * Math.sin((rows ? p.X : p.i) * .5 + p.t * 3), 0, 1);
      return h > .05 ? hsl(48 * h, 1, .22 + .36 * h).concat(.3 + .7 * h) : null;
    },
    'Comet': function (s, p) {
      var tail = 20, head = ((p.t * .4) % 1) * (p.n + tail), d = head - p.i;
      return d >= 0 && d < tail ? rgbOf(s.c1).concat(Math.pow(1 - d / tail, 1.7)) : null;
    },
    'Solid': function (s) { return rgbOf(s.c1).concat(1); }
  };
  var WFX = { Solid: 1, Breathing: 1, Kerze: 1 }; // effects that also drive the white channel of the colour

  // ---- scenarios ----------------------------------------------------------------------------------
  // strips: pm = 'global' (Global verwenden) | 'fix' (Benutzerdefiniert, Fester Wert) | 'led' (Pro LED); ch = channels
  function mkStrips(n, o) {
    var a = [];
    for (var i = 0; i < n; i++) a.push({ name: (o.names && o.names[i]) || 'Streifen ' + (i + 1), chip: o.chip, leds: o.leds[i] || o.leds[0], port: 'Kanal ' + (i + 1), order: o.order, ch: o.ch, drv: o.drv(i), tim: o.tim || '800 kHz · Chipprofil', pm: o.pm || 'global', lim: o.lim ? o.lim[i] || o.lim[0] : 0, perLed: o.perLed || 0, thr: 80, abl: 80, spi: o.spi || 0 });
    return a;
  }
  function pio(i) { return 'PIO' + (i >> 2) + ' · SM ' + (i & 3) + ' · DMA ' + i; }
  function smUse(n, tot) { var a = []; for (var i = 0; i < tot; i++) a.push(i < n ? 1 : 0); return a; }
  var HW2350 = function (nStr, o) {
    return Object.assign({ chip: 'RP2350 · 150 MHz · 2 Kerne', board: 'KNeoPiX V1.5 RP2350', hwid: '0x1312', ls: 'SN74LV8T245', build: 'KNX TP', web: 0, sm: smUse(nStr, 12), dma: 16, dmaUsed: nStr, up: '2 d 09:31', start: 'abgeschlossen', err: '' }, o);
  };
  var EX = 'doc/examples/';
  var SC = {
    live: { label: 'Live', group: 'live' }, // replaced by fromState(/neo/state)
    wgbase: {
      label: 'Wintergarten', group: 'base',
      title: 'Wintergarten: Dachkante links und rechts folgen der HCL-Farbtemperatur von Lichtmanager 1, die Sitzecke nicht. Relais 1 schaltet das LED-Netzteil.',
      bar: { power: 1, bri: 100, layout: 'RGBW', fps: 60, fpsT: 60, hcl: 3400, notes: [] },
      pw: { mode: 'fix', lim: 5000, thr: 80, abl: 80, perLed: 0 },
      strips: mkStrips(2, { names: ['Dachkante links', 'Dachkante rechts + Sitzecke'], chip: 'SK6812/WS2814 RGBW', leds: [150], order: 'GRBW', ch: 4, drv: function () { return 'RMT'; } }),
      segs: [
        { n: 'Dachkante links', a: 0, b: 149, geo: [0], st: 'on', bri: 40, c1: '#000000', w: 255, fx: 'Solid', em: 0, ch: { m: 0 }, hcl: 3400, hclSrc: 'Lichtmanager 1 (global)' },
        { n: 'Dachkante rechts', a: 150, b: 239, geo: [0], st: 'on', bri: 40, c1: '#000000', w: 255, fx: 'Solid', em: 0, ch: { m: 0 }, hcl: 3400, hclSrc: 'Lichtmanager 1 (global)' },
        { n: 'Sitzecke', a: 240, b: 299, geo: [0], st: 'on', bri: 71, c1: '#ff9a40', w: 60, fx: 'Solid', em: 0, ch: { m: 0 }, hcl: 0 }],
      ems: [],
      relays: [{ name: 'LED-Netzteil', st: 1, inv: 0, pin: 'D9', lock: 0, offDelay: 30, minOff: 10 }],
      hw: { chip: 'ESP32-S3 · 240 MHz · 2 Kerne', board: 'KNeoPiX V1.5 ESP32-S3', hwid: '0x1322', ls: 'SN74LV8T245', build: 'KNX TP + WLAN', web: 1, ft: 2.6, ftMin: 2.1, ftMax: 6.9, heap: 186, heapMin: 151, heapTot: 320, rmt: [1, 1, 0, 0], temp: 41.5, up: '3 d 04:12', start: 'abgeschlossen', saved: '3 / 3 Segmente', err: '' },
      chain: null,
      layout: { tpl: 'line', p: { n: 3, dens: 60, gap: 40 }, runs: [{ name: 'Dachkante links', src: 'seg', seg: 0, rev: 0 }, { name: 'Dachkante rechts', src: 'seg', seg: 1, rev: 0 }, { name: 'Sitzecke', src: 'seg', seg: 2, rev: 0 }] }
    },
    kueche: {
      label: 'Küchenzeile', group: 'tpl', src: EX + 'kuechenzeile.json',
      title: 'Ein Streifen mit 120 LEDs (WS2812B) unter den Hängeschränken, ein Segment. Start-Farbe #FFB46B mit 80 %, zwei Szenen. Globale Strombegrenzung 3000 mA.',
      bar: { power: 1, bri: 100, layout: 'RGB', fps: 60, fpsT: 60, hcl: 0, notes: [] },
      pw: { mode: 'fix', lim: 3000, thr: 80, abl: 80, perLed: 60 },
      strips: mkStrips(1, { names: ['Küchenzeile'], chip: 'WS2812B', leds: [120], order: 'GRB', ch: 3, drv: pio }),
      segs: [{ n: 'Küchenzeile', a: 0, b: 119, geo: [0], st: 'on', bri: 80, c1: '#ffb46b', fx: 'Solid', em: 0, ch: { m: 0 }, hcl: 0, scenes: [{ n: 1, c: '#ffd7a8', bri: 100 }, { n: 2, c: '#ff8c40', bri: 35 }] }],
      ems: [], relays: [],
      hw: HW2350(1, { ft: 1.1, ftMin: .9, ftMax: 3.2, heap: 402, heapMin: 388, heapTot: 520, temp: 39.0, saved: '1 / 1 Segment' }),
      chain: null,
      layout: { tpl: 'line', p: { n: 1, dens: 60, gap: 40 }, runs: [{ name: 'Küchenzeile', src: 'seg', seg: 0, rev: 0 }] }
    },
    wintergarten: null, // copy of live with the example hardware, filled in below
    garage: {
      label: 'Garage', group: 'tpl', src: EX + 'garage.json',
      title: 'Drei Streifen (WS2812B) mit je eigenem Netzteil: Einfahrt 120, Wand links 180, Wand rechts 180. Global „Deaktiviert“, jeder Streifen mit eigener Grenze. Torkontakt oder Bewegungsmelder startet EM 1 „Einfahrt“ auf allen drei Segmenten.',
      bar: { power: 1, bri: 100, layout: 'RGB', fps: 60, fpsT: 60, hcl: 0, notes: [] },
      pw: { mode: 'off', perLed: 60 },
      strips: mkStrips(3, { names: ['Einfahrt', 'Wand links', 'Wand rechts'], chip: 'WS2812B', leds: [120, 180, 180], order: 'GRB', ch: 3, drv: pio, pm: 'fix', lim: [4000, 6400, 6400] }),
      segs: [
        { n: 'Einfahrt', a: 0, b: 119, geo: [0], st: 'on', bri: 100, c1: '#ffffff', fx: 'Solid', em: 1, cue: 1, ems: 'run', stopRet: 'Aus', ch: { m: 0 }, hcl: 0 },
        { n: 'Links', a: 120, b: 299, geo: [0], st: 'on', bri: 100, c1: '#ffffff', fx: 'Solid', em: 1, cue: 1, ems: 'run', stopRet: 'Aus', ch: { m: 0 }, hcl: 0 },
        { n: 'Rechts', a: 300, b: 479, geo: [0], st: 'on', bri: 100, c1: '#ffffff', fx: 'Solid', em: 1, cue: 1, ems: 'run', stopRet: 'Aus', ch: { m: 0 }, hcl: 0 }],
      ems: [{ id: 1, name: 'Einfahrt', st: 1, loop: 0, next: 0, stopRet: 'Aus', cues: [{ n: 'Begrüßung', fx: 'Comet', note: 'Weiß', d: 10, f: 1000 }, { n: 'Ausleuchten', fx: 'Solid', note: 'Weiß', c: '#ffffff', d: 0, f: 0 }, { n: 'Nachlauf', fx: 'Breathing', c: '#ffffff', bri: 63, d: 30, f: 3000 }] }],
      relays: [],
      hw: HW2350(3, { ft: 1.9, ftMin: 1.4, ftMax: 4.8, heap: 371, heapMin: 352, heapTot: 520, temp: 37.6, saved: '3 / 3 Segmente' }),
      chain: null,
      layout: { tpl: 'u', p: { w: 300, d: 600 }, runs: [{ name: 'Einfahrt', src: 'seg', seg: 0, rev: 0, on: 1 }, { name: 'Wand links', src: 'seg', seg: 1, rev: 0, on: 1 }, { name: 'Rückwand', src: 'none', rev: 0, on: 1 }, { name: 'Wand rechts', src: 'seg', seg: 2, rev: 0, on: 1 }] },
      sim: 1
    },
    terrasse: {
      label: 'Terrasse · Kette', group: 'tpl', src: EX + 'terrasse.json',
      title: 'Drei Abschnitte mit je eigenem Gerät: 8 Balken × 60 LEDs (SK6812 RGBW), ein Segment als Matrix 60 × 8. Die Effektkette verbindet die drei Matrizen für direkt gewählte Effekte zu einer Fläche 180 × 8. Gezeigt: Abschnitt 1 (Master), Effekt Cylon per KO direkt gewählt.',
      bar: { power: 1, bri: 100, layout: 'RGBW', fps: 58, fpsT: 60, hcl: 0, notes: [] },
      pw: { mode: 'fix', lim: 16000, thr: 80, abl: 80, perLed: 0 },
      strips: mkStrips(8, { names: ['Balken 1', 'Balken 2', 'Balken 3', 'Balken 4', 'Balken 5', 'Balken 6', 'Balken 7', 'Balken 8'], chip: 'SK6812/WS2814 RGBW', leds: [60], order: 'GRBW', ch: 4, drv: pio }),
      segs: [{ n: 'Terrasse 1', a: 0, b: 479, geo: [60, 8, 0, 2], st: 'on', bri: 100, c1: '#ff9040', w: 120, fx: 'Cylon', em: 0, ch: { m: 1, pol: 0, to: 3, total: 1440, off: 0, age: 1 }, hcl: 0, stopRet: 'Aus' }],
      ems: [{ id: 1, name: 'Abendstimmung', st: 1, loop: 1, next: 0, stopRet: 'Aus', cues: [{ n: 'Sonnenunterg.', fx: 'Gradient', bri: 86, d: 600, f: 4000 }, { n: 'Welle', fx: 'Aurora 2D', d: 600, f: 4000 }, { n: 'Glut', fx: 'Fire', bri: 71, d: 600, f: 4000 }] }],
      relays: [],
      hw: HW2350(8, { ft: 6.8, ftMin: 5.9, ftMax: 11.4, heap: 168, heapMin: 142, heapTot: 520, temp: 43.2, up: '5 d 02:17', saved: '1 / 1 Segment' }),
      chain: { seg: 0, total: 1440, peersLit: 1, nodes: [{ n: 'Abschnitt 1', role: 'Master', me: 1, off: 0, len: 480 }, { n: 'Abschnitt 2', role: 'Slave', off: 480, len: 480 }, { n: 'Abschnitt 3', role: 'Slave', off: 960, len: 480 }] },
      layout: { tpl: 'rows', p: { per: 8, len: 100, gap: 30, ggap: 10, leds: 60 }, edge: ['Hauswand', 'Garten'], runs: [{ name: 'Abschnitt 1', src: 'seg', seg: 0, rev: 0 }, { name: 'Abschnitt 2', src: 'chain', base: 480, rev: 0 }, { name: 'Abschnitt 3', src: 'chain', base: 960, rev: 0 }] },
      cap: 'Cylon direkt (ohne EM) über die gemeinsame Matrix 180 × 8: dieses Gerät rendert die Spalten 0–59. Abschnitt 2 und 3 sind angenommen; dieses Gerät sieht ihre LEDs nicht.'
    },
    terrasseem: null, // derived below
    matrix: {
      label: 'Matrix 16 × 16', group: 'tpl', src: EX + 'matrix-panel.json',
      title: 'Ein Panel 16 × 16 (WS2812B), verdrahtet als Schlange in Zeilen (→←→←). Ein Segment als 2D-Matrix mit Plasma Nebula, Start-Helligkeit 50 %. Globale Strombegrenzung 3000 mA.',
      bar: { power: 1, bri: 100, layout: 'RGB', fps: 60, fpsT: 60, hcl: 0, notes: [] },
      pw: { mode: 'fix', lim: 3000, thr: 80, abl: 80, perLed: 60 },
      strips: mkStrips(1, { names: ['Panel 16x16'], chip: 'WS2812B', leds: [256], order: 'GRB', ch: 3, drv: pio }),
      segs: [{ n: 'Matrix', a: 0, b: 255, geo: [16, 16, 0, 1], st: 'on', bri: 50, c1: '#ffb46b', fx: 'Plasma Nebula', em: 0, ch: { m: 0 }, hcl: 0 }],
      ems: [], relays: [],
      hw: HW2350(1, { ft: 2.4, ftMin: 2.0, ftMax: 4.1, heap: 389, heapMin: 371, heapTot: 520, temp: 40.1, saved: '1 / 1 Segment' }),
      chain: null,
      layout: { tpl: 'matrix', p: { cols: 16, rows: 16, pitch: 10 }, runs: [{ name: 'Matrix', src: 'seg', seg: 0, rev: 0 }] }
    },
    wuerfel: {
      label: 'Würfel 3D', group: 'more',
      title: 'Würfel 8 × 8 × 8 (WS2805 RGBCCT) an zwei Streifen, keine Konfigurator-Vorlage. Global „Deaktiviert“, beide Streifen mit eigener Grenze „Pro LED“ 15 mA.',
      bar: { power: 1, bri: 100, layout: 'RGBCCT', fps: 41, fpsT: 50, hcl: 3000, notes: [] },
      pw: { mode: 'off', perLed: 60 },
      strips: mkStrips(2, { names: ['Ebenen 1–4', 'Ebenen 5–8'], chip: 'WS2805 RGBCCT', leds: [256], order: 'RGBWWCW', ch: 5, drv: function (i) { return 'PIO0 · SM ' + i + ' · DMA ' + i; }, pm: 'led', perLed: 15 }),
      segs: [{ n: 'Würfel', a: 0, b: 511, geo: [8, 8, 8, 5], st: 'on', bri: 100, c1: '#ffb36b', w: 255, cw: 120, fx: 'Rainbow', em: 6, cue: 1, ems: 'run', cueEl: 7, ch: { m: 0 }, hcl: 3000, hclSrc: 'eigener Lichtmanager-Kanal 2' }],
      ems: [{ id: 6, name: 'Würfelshow', st: 1, loop: 1, next: 0, cues: [{ n: 'Regenbogen', fx: 'Rainbow', d: 20, f: 800 }, { n: 'Jonglieren', fx: 'Juggle', d: 25, f: 800 }, { n: 'Funkeln', fx: 'Sparkle', d: 25, f: 800 }, { n: 'Atmen', fx: 'Breathing', d: 10, f: 2000 }] }],
      relays: [{ name: 'Netzteil 5 V', st: 1, inv: 0, pin: 'D9', lock: 0 }],
      hw: { chip: 'RP2350 · 150 MHz · 2 Kerne', board: 'KNeoPiX V1 RP2350', hwid: '0x1310', ls: 'TXS0108E', build: 'KNX TP', web: 0, ft: 18.7, ftMin: 11.0, ftMax: 24.9, heap: 96, heapMin: 71, heapTot: 520, sm: smUse(2, 12), dma: 16, dmaUsed: 2, temp: 58.2, up: '0 d 00:52', start: 'abgeschlossen', saved: '1 / 1 Segment', err: '' },
      chain: null,
      layout: { tpl: 'cube', p: { n: 8, pitch: 5 }, runs: [{ name: 'Würfel', src: 'seg', seg: 0, rev: 0 }] },
      cap: 'Die Firmware hat keine eigenen 3D-Effekte: ein Effekt läuft über die LED-Reihenfolge (Ebene für Ebene, Zeilen serpentin).'
    },
    stoerung: {
      label: 'Störfälle', group: 'more',
      title: 'Störfälle, keine Konfigurator-Vorlage: Gerät aus, gesperrtes Segment läuft weiter, Slave ohne Sync, EM suspendiert, Relais in Mindest-Auszeit und ohne Pin, wenig Speicher.',
      bar: { power: 0, bri: 80, layout: 'RGB', fps: 60, fpsT: 60, hcl: 0, notes: ['Gerät aus', 'Tor gesperrt', 'Kette: kein Sync', 'Wenig Speicher'] },
      pw: { mode: 'fix', lim: 3000, thr: 80, abl: 80, perLed: 60 },
      strips: mkStrips(1, { names: ['Fassade + Tor'], chip: 'WS2811', leds: [300], order: 'BRG', ch: 3, drv: function () { return 'PIO0 · SM 0 · DMA 0'; }, tim: '400 kHz · Chipprofil' }),
      segs: [
        { n: 'Fassade', a: 0, b: 149, geo: [0], st: 'off', bri: 0, c1: '#ffffff', fx: 'Pride2015', em: 0, ch: { m: 2, pol: 0, to: 3, total: 450, off: 150, age: 38, timeout: 1 }, hcl: 0 },
        { n: 'Tor', a: 150, b: 299, geo: [0], st: 'lock', locked: 1, bri: 100, c1: '#ff3b30', fx: 'Strobe', em: 0, ch: { m: 0 }, hcl: 0 }],
      ems: [{ id: 8, name: 'Alarm', st: 2, loop: 1, next: 0, cues: [{ n: 'Rot', fx: 'Strobe', c: '#ff0000', d: 5, f: 0 }, { n: 'Weiß', fx: 'Strobe', d: 5, f: 0 }] }],
      relays: [{ name: 'Netzteil', st: 1, inv: 0, pin: 'D9', lock: 0 }, { name: 'Netzteil Fassade', st: 0, inv: 0, pin: 'D10', lock: 12 }, { name: 'Relais 3', st: 0, inv: 1, pin: '', lock: 0, nopin: 1 }],
      hw: { chip: 'RP2040 · 133 MHz · 2 Kerne', board: 'KNeoPiX V1 RP2040', hwid: '0x1300', ls: 'TXS0108E', build: 'KNX TP', web: 0, ft: 2.0, ftMin: 1.6, ftMax: 31.4, heap: 41, heapMin: 9, heapTot: 264, sm: smUse(1, 8), dma: 12, dmaUsed: 1, temp: 46.8, up: '0 d 03:41', start: 'abgeschlossen', saved: '1 / 2 Segmente', err: '' },
      chain: { seg: 0, total: 450, peersLit: 0, nodes: [{ n: 'Fassade Ost', role: 'Master', off: 0, len: 150, unk: 1 }, { n: 'Fassade Mitte', role: 'Slave', me: 1, off: 150, len: 150, timeout: 1 }, { n: 'Fassade West', role: 'Slave', off: 300, len: 150, unk: 1 }] },
      layout: { tpl: 'line', p: { n: 4, dens: 30, gap: 40 }, runs: [{ name: 'Fassade Ost', src: 'chain', base: 0, rev: 0 }, { name: 'Fassade Mitte', src: 'seg', seg: 0, rev: 0 }, { name: 'Fassade West', src: 'chain', base: 300, rev: 0 }, { name: 'Tor', src: 'seg', seg: 1, rev: 0 }] },
      cap: 'Gerät aus, aber das gesperrte Tor läuft weiter: eine Sperre hat Vorrang vor Aus. Fassade Mitte ist nach dem Sync-Timeout aus; die Nachbarn kennt dieses Gerät nicht.'
    },
    neustart: null // derived below
  };
  // Effektmanager available in the base settings (NEOEMEnabled = count of EMs shown in the ETS)
  SC.garage.emCount = 1; SC.terrasse.emCount = 1; SC.wuerfel.emCount = 6; SC.stoerung.emCount = 8;
  (function derive() {
    // Wintergarten as a preview: the example on its own hardware (RP2350, no web server) with the start values.
    var w = clone(SC.wgbase); delete SC.wgbase;
    w.label = 'Wintergarten'; w.group = 'tpl'; w.src = EX + 'wintergarten.json';
    w.title = 'Zwei Streifen (SK6812 RGBW, 150 + 150 LEDs), drei Segmente. Die Dachkanten folgen der HCL-Farbtemperatur von Lichtmanager 1, die Sitzecke nicht. Relais 1 schaltet das gemeinsame Netzteil; globale Grenze 5000 mA. Gezeigt: Start-Werte nach dem Einschalten.';
    w.segs[0].bri = 100; w.segs[1].bri = 100;
    w.strips.forEach(function (s, i) { s.drv = pio(i); });
    w.hw = HW2350(2, { ft: 1.6, ftMin: 1.2, ftMax: 3.9, heap: 377, heapMin: 360, heapTot: 520, temp: 38.2, saved: '3 / 3 Segmente' });
    SC.wintergarten = w; w.emCount = 0;

    // Terrasse while EM 1 runs: the band is suspended, every device renders its own 60 x 8.
    var e = clone(SC.terrasse), s = e.segs[0];
    e.label = 'Terrasse · EM läuft';
    e.title = 'Terrasse, EM 1 „Abendstimmung“ läuft. Die Gruppenadresse 4/4/0 startet EM 1 auf allen drei Geräten. Solange der EM läuft, ist die Effektkette am Segment ausgesetzt: jedes Gerät zeigt den Effekt auf seiner eigenen Matrix 60 × 8, mit eigener Zeitbasis.';
    s.em = 1; s.cue = 2; s.ems = 'run'; s.cueEl = 214; s.fx = 'Solid'; // fx/c1/w = direct state from before the EM start
    e.chain.peersEm = 1;
    e.cap = 'Cue 2 „Welle“ (Aurora 2D) auf jeder Matrix 60 × 8 für sich, nicht auf der gemeinsamen Fläche 180 × 8. Abschnitt 2 und 3 sind angenommen (eigener EM 1, eigene Zeitbasis).';
    SC.terrasseem = e;

    // Restart of the fault example: start delay running, nothing rendered yet.
    var n = clone(SC.stoerung);
    n.label = 'Neustart';
    n.title = 'Neustart, keine Konfigurator-Vorlage: 24 s nach dem Einschalten läuft die Startverzögerung. Bis zu ihrem Ende gibt die Firmware kein Bild aus; Telegramme werden gepuffert und danach der Reihe nach abgearbeitet.';
    n.bar = { power: 1, bri: 80, layout: 'RGB', fps: 0, fpsT: 60, hcl: 0, notes: ['Startphase'] };
    n.segs.forEach(function (x) { x.st = 'wait'; x.locked = 0; }); // a lock is not persisted: nothing is locked during the startup delay
    n.segs[0].ch = { m: 2, pol: 0, to: 3, total: 450, off: 150, age: 0, wait: 1 };
    n.chain.nodes[1].timeout = 0; n.chain.nodes[1].wait = 1;
    n.ems[0].st = 1;
    n.relays = [{ name: 'Netzteil', st: 0, inv: 0, pin: 'D9', lock: 0 }];
    delete n.hw.ets; n.hw.heap = 118; n.hw.heapMin = 104; n.hw.ftMin = 0; n.hw.ftMax = 0; n.hw.ft = 0; n.hw.temp = 0;
    n.hw.up = '0 d 00:00:24'; n.hw.start = 'Startverzögerung · noch 6 s · 3 Telegramme gepuffert'; n.hw.saved = '2 / 2 Segmente'; n.hw.err = '';
    n.cap = 'Startverzögerung: noch keine Ausgabe. Gespeicherte Zustände und gepufferte Telegramme werden erst danach angewendet.';
    SC.neustart = n;
  })();

  // Garage: time-lapse of EM 1 "Einfahrt" (all three segments start with the same telegram).
  // sim seconds: 1 start, cue 1 comet (10 s real) until 5, fade 1 s, cue 2 white holds until the logic sets cue 3
  // after 5 min without motion (12), cue 3 breathing 30 s until 18, last fade-out 1.5 s (fade/2), EM ends, segments off.
  var WHITE = '#ffffff', sim = {};
  function garageSim(t) {
    var T = t % 22, e = function (x) { x = clamp(x, 0, 1); return 1 - Math.pow(1 - x, 3); }, g;
    if (T < 1) g = { cue: 0, lv: 0, el: 0 };
    else if (T < 5) g = { cue: 1, lv: 1, el: (T - 1) * 2.5, ct: (T - 1) * 2.5 };
    else if (T < 5.25) g = { cue: 1, lv: 1 - (T - 5) / .25, el: 10, ct: 10 };
    else if (T < 5.5) g = { cue: 2, lv: (T - 5.25) / .25, el: 0 };
    else if (T < 12) g = { cue: 2, lv: 1, el: (T - 5.5) * 46 };          // 5 min hold squeezed into 6.5 s
    else if (T < 18) g = { cue: 3, lv: 1, el: (T - 12) * 5, ct: (T - 12) * 5 };
    else if (T < 18.3) g = { cue: 3, lv: 1 - (T - 18) / .3, el: 30, ct: 30 + (T - 18) * 5 };   // last cue: fade-out fade/2, then the EM ends
    else g = { cue: 0, lv: 0, el: 0, end: 1 };
    g.T = T;
    g.car = T < 1 ? -80 : T < 7.5 ? -80 + 640 * e((T - 1) / 6.5) : 560;              // car front, cm behind the door
    g.alpha = T > 21 ? 22 - T : T < .4 ? T / .4 : 1;
    sim = g;
  }
  SC.garage.px = function (s, li) {
    if (!sim.cue || sim.lv < .01) return null;
    var q = sc.ems[0].cues[sim.cue - 1], n = s.b - s.a + 1, look = { c1: q.c || '#ffffff' }, c = FX[q.fx](look, { i: li, n: n, t: sim.ct || 0, X: li, Y: 0, W: n, H: 1 });
    if (c) c[3] *= sim.lv * (q.bri || 100) / 100;
    return c;
  };

  var cur = 'live', lastPv = 'terrasseem', tab = 'hw', sc, lay;
  var T0 = performance.now(), last = 0, START = { garage: 3.0 }, power = null, lastLimited = -1e9, lastPw = 0;
  function isPv() { return cur !== 'live'; }
  function curT() { return (performance.now() - T0) / 1000 + (isNaN(tFix) ? START[cur] || 0 : tFix); }

  // ---- live device ---------------------------------------------------------------------------------
  // /neo/state is built in the device's loop task about once per second; /neo/pixels is a copy of the
  // virtual strip (logical order, before HCL, gamma and current limiting); /neo/em holds the EM definitions.
  var LIVE = { st: null, ems: [], emSum: -1, emBusy: 0, px: null, rxAt: 0, ok: 0, err: '', sig: '', power: null, pend: {}, lastSeq: 0, tok: '', unconf: null };
  var LSN = ['ohne', 'TXS0108E', 'SN74HCT125', 'SN74AHCT125', 'SN74LV8T245'];
  var BUILDN = ['KNX TP', 'KNX TP + WLAN', 'KNX IP (WLAN)', 'KNX IP (LAN)'];
  var ERRN = { 1: 'Hardware passt nicht zur ETS', 2: 'keine Hardware in der ETS gewählt', 3: 'GPIO doppelt belegt', 4: 'Streifen nicht angelegt', 5: 'virtueller Streifen nicht angelegt' };
  var WARNN = { 2: 'gespeicherte Daten verworfen (Konfiguration geändert)', 3: 'unbekannter Effekt in der ETS' };
  var RESN = ['Übernommen.', 'Gesperrt: der Befehl wird ignoriert.', 'Startverzögerung läuft: der Befehl wird zurückgestellt und danach abgearbeitet.', 'Ungültiger Befehl: ignoriert.', 'EM nicht startbar (nicht aktiv, ohne Cues oder nicht verfügbar): ignoriert.', 'Szene nicht angelegt: ignoriert.', 'Kein EM läuft: ignoriert.', 'Startverzögerung läuft: der Befehl wird verworfen (Dimmen und Objekte ohne Schreiben-Flag wirken erst danach).'];
  function isLive() { return cur === 'live'; }
  function esc(t) { return String(t).replace(/[&<>"']/g, function (c) { return { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]; }); }
  function hex6(n) { return '#' + ('00000' + (n >>> 0).toString(16)).slice(-6); }
  function hex4(n) { return '0x' + ('000' + (n >>> 0).toString(16).toUpperCase()).slice(-4); }
  function upStr(s) { var d = Math.floor(s / 86400), h = Math.floor(s % 86400 / 3600), m = Math.floor(s % 3600 / 60), x = s % 60; return d + ' d ' + ('0' + h).slice(-2) + ':' + ('0' + m).slice(-2) + (d || h ? '' : ':' + ('0' + x).slice(-2)); }
  function fxName(id) { var n = FX1D.concat(FX2D)[id]; return n || 'Effekt ' + id; }
  var EMPTY = { v: 1, up: 0, leds: 0, bpl: 3, bar: { pw: 1, bri: 0, fps: 0, fpsT: 0, hcl: 0, err: 0, warn: 0, warn2: 0 }, pwr: { mode: 0, lim: 0, thr: 0, abl: 0, perLed: 0, ma: 0, load: 0 },
    hw: { chip: '', mhz: 0, cores: 0, hwid: 0, board: '', ets: 0, etsName: '', ls: 0, build: 0, ft: 0, ftMin: 0, ftMax: 0, temp: 0, heap: 0, heapMin: 0, heapTot: 0, rmtFree: 0, rmtTot: 0, start: 1, startLeft: 0, deferred: 0, savedValid: 0 },
    emCount: 0, emSum: 0, strips: [], segs: [], relays: [], cmds: [], empty: 1 };
  // Converts the device snapshot into the page model (the shape the example data uses).
  function fromState(j, ems) {
    var L = { label: 'Live', group: 'live', title: '', live: 1, emCount: j.emCount || 0 }, i, k;
    var layout = j.bpl === 5 ? 'RGBCCT' : j.bpl === 4 ? 'RGBW' : 'RGB', start = !!j.hw.start;
    L.bar = { power: j.bar.pw, bri: j.bar.bri, layout: layout, fps: j.bar.fps, fpsT: j.bar.fpsT, hcl: j.bar.hcl, notes: [] };
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
    L.hw = { chip: w.chip ? w.chip + ' · ' + w.mhz + ' MHz · ' + w.cores + (w.cores === 1 ? ' Kern' : ' Kerne') : '—', board: esc(w.board || 'unbekannt'), hwid: hex4(w.hwid), ls: LSN[w.ls] || String(w.ls), build: BUILDN[w.build] || 'KNX', web: 1,
      ft: Math.round(w.ft / 100) / 10, ftMin: Math.round(w.ftMin / 100) / 10, ftMax: Math.round(w.ftMax / 100) / 10, heap: Math.round(w.heap / 1024), heapMin: Math.round(w.heapMin / 1024), heapTot: Math.round(w.heapTot / 1024) || 1,
      rmt: used, temp: w.temp / 10, up: upStr(j.up), start: start ? 'abgeschlossen' : w.startLeft > 0 ? 'Startverzögerung · noch ca. ' + Math.ceil(w.startLeft / 1000) + ' s · ' + w.deferred + ' Telegramme gepuffert' : 'Wiederherstellung läuft · ' + w.deferred + ' Telegramme gepuffert',
      saved: !nv ? 'keine Segmente' : (w.savedValid || 0) + ' / ' + nv + (nv > 1 ? ' Segmente' : ' Segment'),
      err: j.bar.err ? 'Fehler ' + j.bar.err + ': ' + (ERRN[j.bar.err] || '') : WARNN[j.bar.warn] || WARNN[j.bar.warn2] || '' };
    if (w.ets) L.hw.ets = w.ets === 255 ? 'eine Auswahl (keine Hardware gewählt)' : esc(w.etsName || 'unbekannt') + ' (' + hex4(w.ets) + ')';
    // Effektkette: this device knows its own part only; the rest of the band is shown as unknown
    L.chain = null;
    for (i = 0; i < L.segs.length && !L.chain; i++) {
      var s = L.segs[i], c = s.ch, len = s.b - s.a + 1;
      if (!c.m || !(c.total > 0)) continue;
      var nodes = [], role = c.m === 1 ? 'Master' : 'Slave', peer = c.m === 1 ? 'Slave' : 'Master oder Slave', off = Math.min(c.off, c.total), end = Math.min(c.total, off + len);
      if (off > 0) nodes.push({ n: 'Kette 0–' + (off - 1), role: peer, off: 0, len: off, unk: 1 });
      nodes.push({ n: 'Segment ' + (i + 1), role: role, me: 1, off: off, len: Math.max(1, end - off), timeout: c.timeout, wait: c.wait });
      if (end < c.total) nodes.push({ n: 'Kette ' + end + '–' + (c.total - 1), role: peer, off: end, len: c.total - end, unk: 1 });
      L.chain = { seg: i, total: c.total, peersLit: 0, nodes: nodes };
    }
    // notes in the status bar
    var n = L.bar.notes;
    if (!j.bar.pw) n.push('Gerät aus');
    if (j.empty) n.push('Verbinde …');
    if (L.hw.ets) n.push('Hardware ≠ ETS');
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
    return [s[0] * 255 / m, s[1] * 255 / m, s[2] * 255 / m, m / 255, r, g, b, w, cw];
  }

  // ---- pixel lookup -------------------------------------------------------------------------------
  function is1D(g) { return !(g[3] > 0) || !(g[0] > 1) || !(g[1] > 1); } // as LedGeometry::is1D: topology 0 whatever the dimensions
  function is2D(g) { return !is1D(g) && (!(g[2] > 1) || g[3] >= 6); } // as LedGeometry::is2D
  function inv(tp, i, w, h) { // local index -> (x, y) for the firmware topologies; tiled is drawn like its column variant
    var x, y;
    if (tp === 1 || tp === 2 || tp === 5) { y = Math.floor(i / w); x = i % w; if (tp === 1 && (y & 1)) x = w - 1 - x; }
    else { x = Math.floor(i / h); y = i % h; if ((tp === 3 || tp === 7) && (x & 1)) y = h - 1 - y; }
    return [x, y];
  }
  function fwd(tp, x, y, w, h) {
    if (tp === 1 || tp === 2 || tp === 5) return y * w + (tp === 1 && (y & 1) ? w - 1 - x : x);
    return x * h + ((tp === 3 || tp === 7) && (x & 1) ? h - 1 - y : y);
  }
  // Segment, strip, EM and relay names exist only in the ETS (no Memory element), so the device cannot show them.
  // The page labels a segment or strip with the name of the arrangement part mapped to it (browser), else by number.
  function partName(src, idx) { var r = lay && lay.runs.filter(function (x) { return x.src === src && x[src === 'seg' ? 'seg' : 'strip'] === idx && x.on !== 0; })[0]; return r && r.name ? r.name : ''; }
  function segLabel(i) { var n = partName('seg', i); return n ? esc(n) : 'Segment ' + (i + 1); } // HTML: names come from imported files
  function stripLabel(i) { var n = partName('strip', i); return n ? esc(n) : 'Streifen ' + (i + 1); }
  function emOf(id) { return sc.ems.filter(function (e) { return e.id === id; })[0]; }
  function emOn(s) { return !!s.em && /run|pause/.test(s.ems || ''); }
  // What a segment shows now: the active cue while an EM runs, else the direct state.
  function lookOf(s) {
    if (sc.live) return { fx: s.fx, c1: s.c1, w: s.w, cw: s.cw, bri: s.bri }; // the device reports the cue state as the segment state
    if (emOn(s)) { var e = emOf(s.em), q = e && e.cues[(s.cue || 1) - 1]; if (q) { var qw = q.w || 0, cct = sc.bar.layout === 'RGBCCT'; return { fx: q.fx, c1: q.c || '#ffffff', w: cct ? qw >> 1 : qw, cw: cct ? qw - (qw >> 1) : 0, bri: s.bri * (q.bri || 100) / 100 }; } } // applyCue sets the cue colour (ETS default #FFFFFF); RGBCCT splits W into WW/CW
    return { fx: s.fx, c1: s.c1, w: s.w, cw: s.cw, bri: s.bri };
  }
  function bandSuspended(s) { return !!(s.ch.m && emOn(s)); } // EffektManager pauses the virtual band while an EM runs
  // li = LED index inside the panel, po = where that panel starts in the chain band (own offset for this device).
  // Returns [R,G,B shown (white mixed in), intensity, r,g,b,w,cw raw channel values for the current estimate] or null.
  function segPx(s, li, t, po) {
    if (sc.live) return po === own(s) && li <= s.b - s.a ? livePx(s.a + li) : null; // other chain devices are not visible from here
    if (s.st === 'off' || s.st === 'wait' || (!sc.bar.power && s.st !== 'lock')) return null; // a lock has priority over global OFF
    if (s.st === 'pause' || s.ems === 'pause') t = 12.3;
    var c;
    if (sc.px) c = sc.px(s, li, t);
    else {
      var ch = s.ch, mine = !ch.m || po === ch.off, look, useBand;
      if (bandSuspended(s)) {
        if (mine || (sc.chain && sc.chain.peersEm)) { look = lookOf(s); useBand = false; if (!mine) t += 37 + po * .011; } // peers: own EM, own clock
        else { look = { fx: s.fx, c1: s.c1, w: s.w, cw: s.cw, bri: s.bri }; useBand = true; }                                      // peers without EM: master's pre-EM state
      } else { look = lookOf(s); useBand = !!ch.m; }
      var band = useBand ? ch.total : s.b - s.a + 1, bi = useBand ? po + li : li, g = s.geo, p = { i: bi, n: band, t: t, X: bi, Y: 0, W: band, H: 1 };
      if (is2D(g)) {
        var xy = inv(g[3], li, g[0], g[1]); p.H = g[1]; p.Y = xy[1];
        if (useBand && (band % g[1] || po % g[1])) return null;                // firmware drops global XY pixels on a band that does not divide
        if (useBand) { p.W = band / g[1]; p.X = po / g[1] + xy[0]; } else { p.W = g[0]; p.X = xy[0]; }
      }
      c = (FX[look.fx] || FX.Solid)(look, p);
      if (!c) return null;
      c[3] *= look.bri / 100;
      if (WFX[look.fx] && sc.bar.layout !== 'RGB') { c[4] = look.w || 0; c[5] = sc.bar.layout === 'RGBCCT' ? look.cw || 0 : 0; }
    }
    if (!c) return null;
    var w = c[4] || 0, cw = c[5] || 0;
    return shown(c[0], c[1], c[2], w, cw, sc.bar.layout).concat(c[3], [c[0], c[1], c[2], w, cw]); // [R,G,B shown, intensity, r,g,b,w,cw]
  }
  function own(s) { return s.ch.m ? s.ch.off : 0; }
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
    if (run.src === 'chain' && sc.chain && sc.chain.peersLit) {
      var nd = sc.chain.nodes.filter(function (n) { return n.off === run.base; })[0];
      if (nd && i < nd.len) return segPx(sc.segs[sc.chain.seg], i, t, run.base);
    }
    return null;
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

  // ---- current estimate: same curve as NeoPixelManager ablScale(), from the rendered frame --------
  function ablScale(req, lim, thr, abl) {
    if (!lim) return 1;
    abl = Math.min(abl, 100);
    var th = Math.floor(lim * thr / 100);
    if (req <= th) return 1;
    var atLimit = th < lim ? abl / 100 : 1;                        // threshold >= 100 %: no soft zone
    if (req > lim) return Math.min(Math.floor(lim * 95 / 100) / req, atLimit);
    return 1 - (req - th) / (lim - th) * (1 - atLimit);
  }
  // Buffer value as PhysicalStrip writes it: gamma LUT (ETS default 2.0), a lit channel never drops to 0.
  var GLUT = []; for (var g8 = 0; g8 < 256; g8++) GLUT.push(Math.floor(Math.pow(g8 / 255, 2.0) * 255 + .5));
  function chMa(v, per) { v = clamp(Math.round(v), 0, 255); if (!v) return 0; var o = GLUT[v] || 1; return Math.floor(o * per * 255 / 65025); } // PowerManager::calculateTotalCurrent
  function stripLimit(st) { return st.pm === 'fix' ? st.lim : st.pm === 'led' ? st.leds * st.perLed : 0; }
  function computePower(t) {
    var P = { strips: [], limited: false }, pw = sc.pw, gb = sc.bar.bri / 100, a0 = 0;
    sc.strips.forEach(function (st) {
      var req = 0, per = pw.perLed ? Math.floor(pw.perLed / (st.spi ? 3 : st.ch)) : st.spi ? 15 : 20; // PowerManager profileForProtocol: Strom pro LED / channels, 0 = LED-type table
      for (var v = a0; v < a0 + st.leds; v++) {
        var s = segAt(v); if (!s) continue;
        var c = segPx(s, v - s.a, t, own(s)); if (!c) continue;
        var k = c[3] * gb;
        req += chMa(c[4] * k, per) + chMa(c[5] * k, per) + chMa(c[6] * k, per) + (st.ch >= 4 ? chMa(c[7] * k, per) : 0) + (st.ch >= 5 ? chMa(c[8] * k, per) : 0);
      }
      a0 += st.leds;
      P.strips.push({ req: req, lim: stripLimit(st), scale: 1 });
    });
    P.strips.forEach(function (p, i) { var st = sc.strips[i]; if (p.lim && !st.spi) p.scale = ablScale(p.req, p.lim, st.thr, st.abl); });
    P.gReq = 0;
    if (pw.mode === 'fix') {
      P.strips.forEach(function (p, i) { if (sc.strips[i].pm === 'global') P.gReq += p.req; });
      P.gScale = ablScale(P.gReq, pw.lim, pw.thr, pw.abl);
      P.strips.forEach(function (p, i) { if (sc.strips[i].pm === 'global' && !sc.strips[i].spi) p.scale = P.gScale; });
    }
    P.total = 0; P.gMa = 0;
    P.strips.forEach(function (p, i) {
      p.ma = p.req * p.scale; if (p.lim && p.ma > p.lim) p.ma = p.lim;
      p.load = p.lim ? Math.min(100, Math.floor(p.ma * 100 / p.lim)) : 0;
      P.total += p.ma; if (sc.strips[i].pm === 'global') P.gMa += p.ma;
      if (p.scale < 1) { P.limited = true; p.cut = 1; }
    });
    if (pw.mode === 'fix' && pw.lim) { if (P.gMa > pw.lim) P.gMa = pw.lim; P.load = Math.min(100, Math.floor(P.gMa * 100 / pw.lim)); }
    return P;
  }
  function powerTick(now, t) {
    if (sc.live) { power = sc.power; return; } // measured by the device
    var prev = power;
    power = computePower(t);
    if (power.limited) lastLimited = now;
    power.active = now - lastLimited < 5000;                           // warning: a frame was limited since the last 5 s sample
    power.strips.forEach(function (p, i) { if (p.cut) p.seen = now; else if (prev && prev.strips[i] && prev.strips[i].seen && now - prev.strips[i].seen < 5000) p.seen = prev.strips[i].seen; });
  }

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
        var x0 = 520, G = { lines: [], areas: [{ x: 0, y: -30, w: x0, h: p.w + 60, drive: 1 }, { x: x0, y: 0, w: p.d, h: p.w, garage: 1, back: runs[2] && runs[2].on !== 0 }], car: 1 };
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
      runs: function (p) { return p.n; }, part: 'Teil',
      build: function (p, runs) { // part length from the LED count of its source
        var G = { lines: [], areas: [] }, x = 0;
        runs.forEach(function (r) {
          var n = runLen(r) || 60, len = n / p.dens * 100;
          G.areas.push({ x: x - 10, y: -16, w: len + 20, h: 32, run: r }); G.lines.push({ run: r, x1: x, y1: 0, x2: x + len, y2: 0, n: n, i0: 0 });
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
  function layoutPlan0(cv, maxH) {
    var T = TPL[lay.tpl], w = cv.clientWidth || 800;
    plan.cv = cv; plan.G = null; plan.chained = false;
    if (cv.id === 'plan') { cv.style.cursor = T.cube ? 'grab' : 'default'; cv.style.touchAction = T.cube ? 'none' : 'auto'; }
    if (T.cube) { plan.sz = sizeCanvas(cv, Math.round(Math.min(maxH + 40, clamp(w * .5, 300, 420)))); return; }
    var G = T.build(lay.p, lay.runs), b;
    G.lines.forEach(function (L) { L.run.N = 0; });
    G.lines.forEach(function (L) { L.run.N = Math.max(L.run.N, L.map ? L.n * lay.p.rows : L.i0 + L.n); });
    G.lines.forEach(function (L) { L.runN = L.run.N; });
    b = bounds(G); var chained = G.areas.some(function (a) { return a.run && nodeOf(a.run); });
    var top = chained ? (lay.edge ? 96 : 76) : (lay.edge ? 36 : 30), bot = lay.edge ? 34 : 22, pad = 22;
    var s = Math.min((w - 2 * pad) / Math.max(1, b.w), maxH / Math.max(1, b.h));
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
  function drawCar(c, X, Y) {
    var p = lay.p, cy = p.w / 2, x = 520 + sim.car, L = 450, W = 180, s = plan.s;
    c.save(); c.globalAlpha = sim.alpha;
    var cl = Math.max(0, Math.min(260, 520 + p.d - 20 - x)), gr = c.createLinearGradient(X(x), 0, X(x + Math.max(cl, 1)), 0); gr.addColorStop(0, 'rgba(255,214,140,.45)'); gr.addColorStop(1, 'rgba(255,214,140,0)');
    if (sim.T < 7.6 && cl > 0) { c.fillStyle = gr; c.beginPath(); c.moveTo(X(x), Y(cy - 60)); c.lineTo(X(x + cl), Y(cy - 60 - cl * .23)); c.lineTo(X(x + cl), Y(cy + 60 + cl * .23)); c.lineTo(X(x), Y(cy + 60)); c.fill(); }
    c.fillStyle = '#c4ccd3'; c.strokeStyle = '#8f99a2'; c.lineWidth = 1.2;
    rr(c, X(x - L), Y(cy - W / 2), L * s, W * s, 34 * s); c.fill(); c.stroke();
    c.fillStyle = '#e9eef2'; rr(c, X(x - 170), Y(cy - W / 2 + 18), 70 * s, (W - 36) * s, 10 * s); c.fill();       // windscreen
    rr(c, X(x - 390), Y(cy - W / 2 + 22), 45 * s, (W - 44) * s, 8 * s); c.fill();                                  // rear window
    c.fillStyle = '#7d868e'; [[x - 110, cy - W / 2 - 6], [x - 110, cy + W / 2 - 6], [x - 360, cy - W / 2 - 6], [x - 360, cy + W / 2 - 6]].forEach(function (w) { rr(c, X(w[0] - 32), Y(w[1]), 64 * s, 12 * s, 3); c.fill(); });
    c.restore();
  }
  // KNX group telegrams from the master to all slaves; returns the slaves a telegram just reached
  function pillsAndBus(c, pts, yBus, t) {
    var master = pts.filter(function (p) { return p.node.role === 'Master'; })[0], xs = pts.map(function (p) { return p.x; });
    c.strokeStyle = LINE; c.lineWidth = 2; c.setLineDash([]);
    c.beginPath(); c.moveTo(Math.min.apply(null, xs), yBus); c.lineTo(Math.max.apply(null, xs), yBus); c.stroke();
    pts.forEach(function (p) {
      var broken = p.node.timeout, unk = p.node.unk && !broken;
      c.strokeStyle = broken ? BAD : unk ? LINE : FAINT; c.setLineDash(broken || unk ? [4, 4] : []);
      c.beginPath(); c.moveTo(p.x, yBus); c.lineTo(p.x, p.y); c.stroke(); c.setLineDash([]);
      if (broken) { var my = (yBus + p.y) / 2; c.strokeStyle = BAD; c.lineWidth = 2; c.beginPath(); c.moveTo(p.x - 5, my - 5); c.lineTo(p.x + 5, my + 5); c.moveTo(p.x + 5, my - 5); c.lineTo(p.x - 5, my + 5); c.stroke(); }
    });
    c.font = '11px sans-serif'; c.fillStyle = FAINT; c.textAlign = 'left'; c.textBaseline = 'bottom';
    if (xs.length > 1) c.fillText(fit(c, 'KNX · Sync-KO (Gruppenadresse)', Math.max(60, Math.abs(xs[1] - xs[0]) - 20)), Math.min.apply(null, xs) + 6, yBus - 3);
    var arrived = {};
    if (master && !master.node.unk) {
      var ph = (t % 3) / 1.1; // heartbeat every 3 s; a change is sent at once (min. 300 ms apart)
      pts.forEach(function (p) {
        if (p === master) return;
        var l1 = master.y - yBus, l2 = Math.abs(p.x - master.x), l3 = p.y - yBus, L = l1 + l2 + l3, d = clamp(ph, 0, 1) * L, x, y;
        if (ph > 1) { if (ph < 1.35) arrived[p.x] = 1; return; }
        if (d < l1) { x = master.x; y = master.y - d; } else if (d < l1 + l2) { x = master.x + Math.sign(p.x - master.x) * (d - l1); y = yBus; } else { x = p.x; y = yBus + (d - l1 - l2); }
        c.fillStyle = GREEN; c.globalAlpha = .22; c.beginPath(); c.arc(x, y, 9, 0, 6.283); c.fill(); c.globalAlpha = 1; c.beginPath(); c.arc(x, y, 4, 0, 6.283); c.fill();
      });
    }
    return arrived;
  }
  function nodeBox(c, p, x, y, w, h, lines, arrived, ignored) {
    var n = p.node;
    rr(c, x, y, w, h, 7);
    c.fillStyle = n.timeout ? BADBG : n.me ? GREENT : '#fff'; c.fill();
    if (arrived && !n.me) { c.globalAlpha = .18; c.lineWidth = 6; c.strokeStyle = ignored ? FAINT : GREEN; c.stroke(); c.globalAlpha = 1; }
    c.lineWidth = n.me || n.timeout ? 2 : 1.2; c.strokeStyle = n.timeout ? BAD : n.me ? GREEN : FAINT;
    c.setLineDash(n.me || n.timeout ? [] : [4, 3]); c.stroke(); c.setLineDash([]);
    c.textAlign = 'center'; c.textBaseline = 'middle';
    lines.forEach(function (l, i) { c.font = (i === 0 ? '600 12px' : '11px') + ' sans-serif'; c.fillStyle = i === 0 ? INK : (n.timeout && i === lines.length - 1 ? BADFG : MUTE); c.fillText(fit(c, l, w - 10), x + w / 2, y + 12 + i * 15); });
  }
  function drawPlan0(t) {
    var cv = plan.cv, c = cv.getContext('2d'), z = plan.sz, G = plan.G;
    c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = isPv() ? '#fcfbfe' : STAGE; c.fillRect(0, 0, z.w, z.h);
    if (!G) return drawCube(c, z, t);
    var s = plan.s, X = function (x) { return plan.ox + x * s; }, Y = function (y) { return plan.oy + y * s; };
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
      if (nd && a.run.src !== 'none' && bandSuspended(sc.segs[sc.chain.seg]) && (nd.me || sc.chain.peersEm)) { // EM: every device on its own panel
        var g0 = sc.segs[sc.chain.seg].geo; c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center';
        c.fillText(fit(c, 'EM ' + sc.segs[sc.chain.seg].em + ' · eigene ' + (is2D(g0) ? 'Matrix ' + g0[0] + ' × ' + g0[1] : 'LEDs') + (nd.me ? '' : ' (angenommen)'), a.w * s - 8), X(a.x + a.w / 2), Y(a.y) + 8);
      }
      if (!nd) { c.fillStyle = INK; c.font = '600 11px sans-serif'; c.textAlign = 'left'; c.fillText(fit(c, a.run.name + (a.run.src === 'none' ? ' · nicht belegt' : ''), Math.max(30, a.w * s - 4)), X(a.x) + 4, Y(a.y) - 9); }
    });
    if (lay.edge) { c.fillStyle = FAINT; c.font = '11px sans-serif'; c.textAlign = 'center'; c.fillText('↑ ' + lay.edge[0], X(plan.b.x + plan.b.w / 2), plan.top - 14); c.fillText('↓ ' + lay.edge[1], X(plan.b.x + plan.b.w / 2), Y(plan.b.y + plan.b.h) + 18); }
    if (G.car && sc.sim && sim.car !== undefined) drawCar(c, X, Y);
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
      var susp = bandSuspended(sc.segs[sc.chain.seg]);
      var pts = G.areas.filter(function (a) { return a.run && nodeOf(a.run); }).map(function (a) { return { x: X(a.x + a.w / 2), y: 38, node: nodeOf(a.run), w: a.w * s + 20 }; });
      pts.forEach(function (p, i) { var d = Math.min(i ? p.x - pts[i - 1].x : 1e9, i < pts.length - 1 ? pts[i + 1].x - p.x : 1e9); p.w = Math.min(p.w, d - 8, 2 * Math.min(p.x, z.w - p.x) - 4); });
      var arrived = pillsAndBus(c, pts, 22, t);
      pts.forEach(function (p) {
        var n = p.node, txt = n.n + ' · ' + n.role + (n.me ? ' · dieses Gerät' : n.unk ? ' · unbekannt' : ''), w;
        c.font = '600 11px sans-serif';
        if (c.measureText(txt).width + 18 > p.w) txt = n.n + ' · ' + n.role;
        w = Math.min(c.measureText(txt).width + 18, p.w);
        nodeBox(c, p, p.x - w / 2, 38, w, 22, [], arrived[p.x], susp && sc.chain.peersEm);
        c.font = '600 11px sans-serif'; c.fillStyle = n.timeout ? BADFG : INK; c.textAlign = 'center'; c.fillText(fit(c, txt, w - 10), p.x, 49);
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
    c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = isPv() ? '#fcfbfe' : STAGE; c.fillRect(0, 0, z.w, z.h);
    var yBus = 26, yNode = 56, nh = 52, yBand = 156, bh = 64, H2 = is2D(cs.geo) && ch.total % cs.geo[1] === 0 ? cs.geo[1] : 0, gapX = susp ? 8 : 1;
    var pts = ch.nodes.map(function (n) { var x0 = pad + n.off / ch.total * bw, x1 = pad + (n.off + n.len) / ch.total * bw; return { x: (x0 + x1) / 2, x0: x0, x1: x1, y: yNode, node: n }; });
    var arrived = pillsAndBus(c, pts, yBus, t);
    pts.forEach(function (p) {
      var n = p.node, w = Math.min(p.x1 - p.x0 - 10, 250), last;
      if (n.me) last = n.timeout ? 'kein Sync seit ' + cs.ch.age + ' s' : n.wait ? 'Überwachung ab Startende' : cs.ch.none ? (n.role === 'Master' ? 'noch nichts gesendet' : 'noch kein Sync empfangen') : susp ? 'EM ' + cs.em + ' läuft · Band ausgesetzt' : (n.role === 'Master' ? 'Sync gesendet vor ' : 'Sync empfangen vor ') + cs.ch.age + ' s';
      else last = n.unk ? 'Zustand unbekannt' : susp && ch.peersEm ? 'eigener EM · ignoriert Sync' : 'meldet nichts zurück';
      nodeBox(c, p, p.x - w / 2, yNode, w, nh, [n.n, n.role + (n.me ? ' · dieses Gerät' : ''), last], arrived[p.x], susp && ch.peersEm);
      c.strokeStyle = n.me ? GREEN : LINE; c.lineWidth = 1; c.setLineDash([2, 3]); c.beginPath(); c.moveTo(p.x, yNode + nh); c.lineTo(p.x, yBand - 4); c.stroke(); c.setLineDash([]);
      var lit = n.me ? cs.st !== 'off' && cs.st !== 'wait' && (!!sc.bar.power || cs.st === 'lock') : ch.peersLit, x0 = p.x0 + gapX, x1 = p.x1 - gapX;
      if (!n.me && !ch.peersLit) {
        c.save(); rr(c, x0, yBand, x1 - x0, bh, 4); c.clip(); c.fillStyle = '#f3f4f1'; c.fillRect(x0, yBand, x1 - x0, bh);
        c.strokeStyle = '#e3e6e0'; c.lineWidth = 6; for (var hx = x0 - bh; hx < x1; hx += 14) { c.beginPath(); c.moveTo(hx, yBand + bh); c.lineTo(hx + bh, yBand); c.stroke(); }
        c.restore(); c.fillStyle = MUTE; c.font = '11px sans-serif'; c.textAlign = 'center'; c.fillText('unbekannt', p.x, yBand + bh / 2);
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
      c.fillText(fit(c, lab + (!n.me && ch.peersLit ? ' · angenommen' : ''), x1 - x0 - 6), p.x, yBand + bh + 16);
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
    if (!e) return 'EM ' + s.em + '<small>nicht in der ETS-Konfiguration</small>';
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
    $('bar').innerHTML = (isPv() ? '<div class="np-b"><span class="np-pill" style="background:var(--np-pv-d);color:#fff">Vorschau</span></div>' : '') +
      '<div class="np-b"><span>Gerät</span>' + (b.power ? pill('ok', 'Ein') : pill('bad', 'Aus')) + '</div>' +
      '<div class="np-b"><span>Helligkeit</span><b>' + b.bri + ' %</b></div>' +
      '<div class="np-b"><span>LEDs</span><b>' + fmt(sc.strips.reduce(function (a, s) { return a + s.leds; }, 0)) + '</b><em>' + sc.strips.length + ' Streifen · ' + b.layout + '</em></div>' +
      '<div class="np-b"><span>Bildrate</span><b class="' + (b.fps && b.fps < b.fpsT * .8 ? 'np-warn' : '') + '">' + (b.fps || '—') + '</b><em>/ ' + b.fpsT + ' fps</em></div>' +
      '<div class="np-b"><span>Strom</span>' + curTxt + '</div>' +
      '<div class="np-b"><span>HCL</span><b>' + (b.hcl ? fmt(b.hcl) + ' K' : '<span class="np-f">aus</span>') + '</b></div>' +
      '<div class="np-b notes"><span>Hinweise</span>' + (notes.length ? notes.map(function (n) { return pill(/aus|≠|Sync|Fehler/.test(n) ? 'bad' : n === 'Verbinde …' ? 'idle' : 'warn', n); }).join('') : pill('ok', 'keine')) + '</div>';
  }

  function renderPipe() {
    var tot = sc.strips.reduce(function (a, s) { return a + s.leds; }, 0), h = '', a = 0;
    function row(l, sub, parts) { return '<div class="np-pipe-l"><b>' + l + '</b><span>' + sub + '</span></div><div class="np-row">' + parts + '</div>'; }
    function part(w, cls, t1, t2, tip) { return '<div class="' + cls + '" style="flex:' + w + ' 1 0" title="' + (tip || '') + '">' + t1 + '<small>' + t2 + '</small></div>'; }
    h += row('Physikalische Streifen', sc.strips.length + ' Anschlüsse', sc.strips.map(function (s, i) { return part(s.leds, 't' + (i % 8), '<b>S' + (i + 1) + '</b> ' + stripLabel(i), s.leds + ' LEDs', s.chip + ' an ' + s.port); }).join(''));
    h += '<div class="np-arrow">↓ hintereinander gehängt</div>';
    h += row('Virtueller Streifen', fmt(tot) + ' LEDs · ' + sc.bar.layout, sc.strips.map(function (s, i) { var r = part(s.leds, 't' + (i % 8), a + '–' + (a + s.leds - 1), 'S' + (i + 1)); a += s.leds; return r; }).join(''));
    h += '<div class="np-arrow">↓ aufgeteilt in Segmente</div>';
    h += row('Segmente', sc.segs.length + (sc.segs.length > 1 ? ' Segmente' : ' Segment'), sc.segs.map(function (s, i) {
      var g = s.geo; return part(s.b - s.a + 1, 't' + SEGT(i), '<b>' + (i + 1) + '</b> ' + segLabel(i), s.a + '–' + s.b + ' · ' + geoShort(g) + (!is1D(g) ? ' · ' + TOPO[g[3]] : ''));
    }).join(''));
    if (sc.chain) {
      var ch = sc.chain, cs = sc.segs[ch.seg];
      h += '<div class="np-arrow">↓ Segment ' + (ch.seg + 1) + ' ist Teil einer Effektkette' + (bandSuspended(cs) ? ' · ausgesetzt, solange EM ' + cs.em + ' läuft' : '') + '</div>';
      h += row('Effektkette', fmt(ch.total) + ' LEDs über alle Geräte', ch.nodes.map(function (n) { return part(n.len, n.me ? 't' + NODET[ch.nodes.indexOf(n) % 4] + ' me' : 'peer', n.n + (n.me ? ' · dieses Gerät' : ''), n.off + '–' + (n.off + n.len - 1)); }).join(''));
    }
    $('pipe').innerHTML = h;
  }

  function simText() {
    var cues = sc.ems[0].cues, s = !sim.cue ? (sim.end ? 'EM beendet, Segmente aus (Rückkehr: Aus)' : 'wartet auf Torkontakt') : 'Cue ' + sim.cue + ' „' + cues[sim.cue - 1].n + '“' + (sim.cue === 2 ? ' hält, bis die Logik nach 5 min ohne Bewegung Cue 3 setzt' : '');
    return '<b>Simulation im Zeitraffer</b> · alle drei Segmente: ' + s;
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
    if (sc.sim) h += '<span id="simcap">' + simText() + '</span>';
    else if (sc.cap && lay.tpl === sc.layout.tpl) h += '<span>' + sc.cap + '</span>';
    $('plan-cap').innerHTML = h;
    $('lay-name').textContent = T.name + (lay.saved ? ' · in diesem Browser gespeichert' : isPv() ? ' · Beispiel' : ' · Standard');
  }

  function renderSegs() {
    var h = '<table class="np-tbl"><tr><th>Segment</th><th>Bereich</th><th>Form</th><th>Zustand</th><th>Hell.</th><th>Farbe</th><th>Effekt</th><th>Effektmanager</th><th>Kette</th><th>HCL</th></tr>';
    sc.segs.forEach(function (s, i) {
      var ch = '<span class="np-f">—</span>', lk = lookOf(s);
      if (s.ch.m) ch = (s.ch.timeout ? pill('bad', 'Slave · Timeout') : bandSuspended(s) ? (s.ch.m === 1 ? 'Master' : 'Slave') + ' ' + pill('warn', 'ausgesetzt (EM)') : (s.ch.m === 1 ? 'Master' : 'Slave')) + '<small>' + fmt(s.ch.total) + ' LEDs · Teil ' + s.ch.off + '–' + (s.ch.off + s.b - s.a) + '</small>';
      h += '<tr><td>' + badge(SEGT(i), i + 1) + segLabel(i) + '</td><td class="np-num">' + s.a + '–' + s.b + '<small>' + (s.b - s.a + 1) + ' LEDs</small></td><td>' + geoShort(s.geo) + (!is1D(s.geo) ? '<small>' + TOPO[s.geo[3]] + '</small>' : '') + '</td>' +
        '<td>' + stPill(s) + '</td><td class="np-num">' + s.bri + ' %</td>' +
        '<td>' + swatch(s.c1, s.w, s.cw) + (s.c2 ? swatch(s.c2) : '') + '<small>' + s.c1.toUpperCase() + (s.w ? ' + W ' + s.w : '') + (s.cw ? ' · CW ' + s.cw : '') + '</small></td>' +
        '<td>' + (emOn(s) ? lk.fx + '<small>aus Cue ' + s.cue + '</small>' : s.fx + (s.scenes ? '<small>Szenen ' + s.scenes.map(function (q) { return q.n; }).join(', ') + ' angelegt</small>' : '')) + '</td><td>' + emText(s) + '</td><td>' + ch + '</td><td>' + (s.hcl ? fmt(s.hcl) + ' K<small>' + (s.hclSrc || '') + '</small>' : '<span class="np-f">—</span>') + '</td></tr>';
    });
    h += '</table><div class="np-legend"><span>' + pill('ok', 'Ein') + '</span><span>' + pill('warn', 'Pausiert') + ' EM angehalten</span><span>' + pill('bad', 'Gesperrt') + ' Sperre-KO aktiv, ignoriert Befehle</span><span>' + pill('idle', 'Aus') + '</span>' + (sc.segs.some(function (s) { return s.st === 'wait'; }) ? '<span>' + pill('idle', 'wartet auf Startende') + ' noch keine Ausgabe</span>' : '') + '<span>Farbfeld = eingestellte Farbe, Weiß im Weißton des Streifens</span></div>';
    if (sc.segs.some(function (s) { return s.st === 'lock'; })) h += '<div class="np-note warn"><b>Gesperrt:</b> Ein gesperrtes Segment ignoriert alle steuernden KOs und Sync-Telegramme, bis die Sperre aufgehoben wird. Effekt und EM laufen weiter, auch wenn das Gerät ausgeschaltet wird: die Sperre hat Vorrang vor Aus.</div>';
    $('segs').innerHTML = h;
  }

  function cueEl(s) { // elapsed seconds in the active cue (device: EffektManagerController::cueElapsedMs)
    if (sc.sim) return sim.el || 0;
    if (sc.live) return s.cueEl + (s.ems === 'run' ? (performance.now() - LIVE.rxAt) / 1000 : 0);
    return (s.cueEl || 0) + (performance.now() - T0) / 1000;
  }
  function renderEm() {
    if (sc.live && LIVE.emSum < 0 && sc.emCount) { $('em').innerHTML = '<div class="np-empty">Effektmanager werden geladen …</div>'; return; }
    if (!sc.ems.length) { $('em').innerHTML = '<div class="np-empty"><b>Kein Effektmanager angelegt.</b><br>Ein Effektmanager spielt Cues (Effekt, Farbe, Dauer, Überblendung) nacheinander auf einem Segment ab.</div>'; return; }
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
        h += '<div class="np-cue t' + CUET(k) + cls + '" style="flex-grow:' + grow + '"><b>' + (k + 1) + ' · ' + q.n + '</b><small>' + q.fx + (q.note ? ' · ' + q.note : '') + (q.bri ? ' · ' + q.bri + ' %' : '') + '</small>' +
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
      if (e.st === 2) h += '<div class="np-note warn">Suspendiert: Der EM ist in der ETS vorhanden, startet aber nicht, bis er wieder aktiv geschaltet wird.</div>';
      if (blocked) h += '<div class="np-note">Cue ' + blocked + ' hält (Dauer 0): weiter geht es nur per KO „Cue setzen“ oder „Cue +“; ein solcher Sprung ist ein harter Wechsel ohne Überblendung.' + (sc.sim ? ' Im Beispiel setzt die Logik 5 min nach der letzten Bewegung Cue 3.' : '') + '</div>';
      h += '</div>';
    });
    var chs = sc.chain && sc.segs[sc.chain.seg];
    h += '<div class="np-note"><b>Position innerhalb der Cue</b> (Zeit und Balken): verstrichene Zeit in der laufenden Cue, beim Pausieren eingefroren.' +
      ' „ab“ zählt die Überblendung mit: die nächste Cue startet nach der halben Überblendzeit.' +
      (sc.sim ? ' Die Garage zeigt eine Simulation im Zeitraffer; die markierte Cue folgt der Animation.' : '') +
      (chs ? '<br><b>Effektkette:</b> Solange ein EM auf dem Segment läuft, ist die Kette dort ausgesetzt: das Gerät zeigt den EM nur auf seiner eigenen ' + (is2D(chs.geo) ? 'Matrix ' + chs.geo[0] + ' × ' + chs.geo[1] : 'LED-Reihe') + '. Cues gehen nicht über die Kette; jedes Gerät braucht seinen eigenen EM.' : '') + '</div>';
    $('em').innerHTML = h;
  }

  function renderChain() {
    var ch = sc.chain;
    var rules = '<div class="np-note info"><b>Effektkette und Effektmanager.</b> Läuft auf einem Segment ein EM, setzt der EM die Kette an diesem Segment aus: das Gerät rendert nur seine eigene Matrix bzw. LED-Reihe, nicht seinen Teil der gemeinsamen Fläche. Nach dem EM-Stop gilt die Kette wieder.' +
      '<br><b>Master mit laufendem EM:</b> sendet weiter den Zustand von vor dem EM-Start (Effekt, Tempo, Farbe); Ein/Aus und Helligkeit bleiben live. Cue-Wechsel gehen nicht über die Kette.' +
      '<br><b>Slave mit eigenem laufendem EM:</b> ignoriert die Sync-Telegramme. Jedes Telegramm zählt aber als Lebenszeichen, der Sync-Timeout greift also nicht. Ein Slave ohne eigenen EM übernimmt den Zustand des Masters und rendert ihn auf der gemeinsamen Fläche.</div>';
    if (!ch) { $('chain').innerHTML = '<div class="np-empty"><b>Kein Segment in einer Effektkette.</b><br>In einer Effektkette spielen mehrere Geräte einen gemeinsamen Effekt: Jedes Gerät rendert seinen Teil eines gemeinsamen Bands, der Master sendet Ein/Aus, Effekt, Tempo, Farbe und Helligkeit an die Slaves.</div>' + rules; return; }
    var s = sc.segs[ch.seg], me = ch.nodes.filter(function (n) { return n.me; })[0], H2 = is2D(s.geo) && ch.total % s.geo[1] === 0 ? s.geo[1] : 0, h = '', susp = bandSuspended(s);
    if (!me) { $('chain').innerHTML = '<div class="np-empty">Kettendaten unvollständig.</div>'; return; }
    if (susp) h += '<div class="np-note warn"><b>Kette ausgesetzt:</b> EM ' + s.em + ' läuft auf diesem Gerät. Es zeigt den EM nur auf seiner eigenen ' + (H2 ? 'Matrix ' + s.geo[0] + ' × ' + s.geo[1] + ', nicht auf der gemeinsamen Fläche ' + ch.total / H2 + ' × ' + H2 : 'LED-Reihe') + '. ' +
      (me.role === 'Master' ? 'Die Slaves bekommen weiter den Zustand von vor dem EM-Start; ' + (ch.peersEm ? 'im Beispiel läuft auf ihnen derselbe EM 1 (gestartet über dieselbe Gruppenadresse), deshalb ignorieren sie die Telegramme.' : 'ohne eigenen EM zeigen sie diesen Zustand auf der gemeinsamen Fläche.') : 'Sync-Telegramme vom Master werden ignoriert, halten aber die Timeout-Überwachung am Leben.') + '</div>';
    if (me.wait) h += '<div class="np-note info"><b>Startphase:</b> Die Sync-Überwachung beginnt erst nach der Startverzögerung; bis dahin zählt kein Timeout.</div>';
    if (me.timeout) h += '<div class="np-note bad"><b>Kein Sync vom Master ' + (s.ch.none ? 'seit dem Start' : 'seit ' + s.ch.age + ' s') + '</b> (Timeout ' + s.ch.to * 10 + ' s). Das Segment „' + segLabel(ch.seg) + '“ wurde ausgeschaltet; das nächste Sync-Telegramm schaltet es wieder nach Vorgabe des Masters.</div>';
    h += '<div class="np-card"><div class="np-card-h"><h3>' + segLabel(ch.seg) + ' · ' + me.role + '</h3>' + (me.timeout ? pill('bad', 'Timeout') : me.wait ? pill('idle', 'Startphase') : susp ? pill('warn', 'ausgesetzt (EM ' + s.em + ')') : pill('ok', me.role === 'Master' ? 'sendet' : 'empfängt')) + '<span class="np-sp"></span><span class="np-f">Namen und Lage der anderen Geräte stammen aus der Anordnung</span></div><canvas class="np-stage" id="chaincv"></canvas>' +
      '<div class="np-cap">' + ledLegend() + '</div><div class="np-cap"><span class="np-key"><i style="background:' + GREEN + '"></i>Sync-Telegramm alle 3 s (Lebenszeichen), bei Änderung sofort</span><span>grün umrandet = dieses Gerät</span><span>gestrichelt = andere Geräte, von hier nicht überwacht</span>' +
      (ch.peersLit ? '<span><b>Die anderen Geräte sind angenommen:</b> dieses Gerät sieht ihre LEDs nicht.' + (susp && ch.peersEm ? ' Angenommen: EM 1 läuft auch dort, mit eigener Zeitbasis.' : '') + '</span>' : '') + '</div></div>';
    var pl = s.ch.pl, plTxt = pl ? fxName(pl[1]) + ' ' + hex6(pl[3] << 16 | pl[4] << 8 | pl[5]).toUpperCase() + (pl[6] ? ' + W ' + pl[6] : '') + (pl[8] ? ' + CW ' + pl[8] : '') + ', ' + (pl[0] & 1 ? 'Ein' : 'Aus') + ' · ' + Math.round(pl[7] * 100 / 255) + ' %' : '';
    var sent = me.role === 'Master' && sc.live ? (s.ch.none ? 'noch nichts gesendet' : 'vor ' + s.ch.age + ' s · ' + (susp ? 'Zustand vor dem EM-Start: ' : '') + plTxt) : me.role === 'Master' ? (susp ? 'vor ' + s.ch.age + ' s · Zustand vor dem EM-Start (' + s.fx + ' ' + s.c1.toUpperCase() + (s.w ? ' + W ' + s.w : '') + '), Ein/Aus und Helligkeit live' : 'vor ' + s.ch.age + ' s · ' + s.fx + ' ' + s.c1.toUpperCase() + (s.w ? ' + W ' + s.w : '')) : '';
    h += '<div class="np-two"><div class="np-card"><div class="np-card-h"><h3>Was dieses Gerät weiß</h3></div><div class="np-card-b np-kv">' +
      '<i>Rolle</i><b>' + me.role + (me.role === 'Slave' ? ' · ' + ['Sync übernimmt immer', 'Lokal hat Vorrang'][s.ch.pol] : '') + '</b>' +
      '<i>Gesamtlänge (ETS)</i><b>' + fmt(ch.total) + ' LEDs</b><i>Eigener Versatz (ETS)</i><b>' + s.ch.off + ' · Teil ' + s.ch.off + '–' + (s.ch.off + s.b - s.a) + '</b>' +
      (H2 ? '<i>2D über die Kette</i><b>gemeinsame Matrix ' + ch.total / H2 + ' × ' + H2 + ' · eigene Spalten ' + s.ch.off / H2 + '–' + ((s.ch.off + s.b - s.a + 1) / H2 - 1) + '</b>' : '') +
      '<i>Band</i><b>' + (susp ? pill('warn', 'ausgesetzt') + ' solange EM ' + s.em + ' läuft' + (H2 ? ' · eigene Matrix ' + s.geo[0] + ' × ' + s.geo[1] : '') : 'aktiv') + '</b>' +
      (me.role === 'Slave' ? '<i>Timeout</i><b>' + (s.ch.to ? s.ch.to * 10 + ' s' : 'aus') + '</b><i>Letzter Sync</i><b class="' + (me.timeout ? 'np-bad' : '') + '">' + (me.wait || s.ch.none ? 'noch keiner seit dem Start' : 'vor ' + s.ch.age + ' s') + '</b>' : '<i>Zuletzt gesendet</i><b>' + sent + '</b>') +
      '<i>Andere Geräte</i><b class="np-m">' + (me.role === 'Master' ? 'unbekannt: Slaves senden nichts zurück' : 'unbekannt: nur Telegramme des Masters kommen an') + '</b></div></div>' +
      '<div class="np-card"><div class="np-card-h"><h3>Was über die Kette geht</h3></div><div class="np-card-b" style="font-size:.84em">Sync-KO: 14 Byte (DPT 16), davon 9 belegt:<div class="np-bytes">' +
      ['Ein/Aus', 'Effekt', 'Tempo', 'Rot', 'Grün', 'Blau', 'Weiß (WW)', 'Helligkeit', 'Kaltweiß (CW)'].map(function (b, i) { return '<span class="t' + [0, 1, 1, 2, 2, 2, 2, 3, 2][i] + '">' + b + '</span>'; }).join('') + '</div>' +
      '<div class="np-m">Nicht übertragen: Zeit bzw. Phase, Effekt-Parameter aus der ETS (z. B. Farbton bei Cylon) und die Cues eines EM. Jedes Gerät rechnet das Band mit eigener Uhr und eigenen Parametern.</div></div></div></div>' + rules;
    $('chain').innerHTML = h;
  }

  function renderStrips() {
    var P = power, pw = sc.pw, h = '';
    if (!P) return;
    var spi = sc.strips.some(function (s) { return s.spi; }), own = sc.strips.some(stripLimit), ownS = sc.strips.filter(stripLimit)[0];
    h += '<div class="np-card"><div class="np-card-h"><h3>Strom</h3>' + (P.active ? pill('warn', 'Begrenzung aktiv') : pill('ok', 'keine Begrenzung')) + '<span class="np-sp"></span><span class="np-f">' + (sc.live ? 'aus dem Ausgabepuffer berechnet (nach HCL und Gamma), nicht gemessen' : 'Vorschau: Schätzung ohne HCL') + '</span></div><div class="np-card-b np-kv">' +
      '<i>Globale Strombegrenzung</i><b>' + (pw.mode === 'fix' ? 'Fester Wert · ' + fmt(pw.lim) + ' mA' : pw.mode === 'led' ? 'Pro LED · ' + pw.perLed + ' mA je LED = ' + fmt(pw.lim) + ' mA' : 'Deaktiviert') + '</b>' +
      (pw.mode !== 'off' ? '<i>Gesamt</i><b>' + meter(P.load, P.active ? 'warn lg' : 'lg', pw.thr) + fmt(P.gMa) + ' mA von ' + fmt(pw.lim) + ' mA · ' + P.load + ' %' + (P.gScale < 1 ? ' <span class="np-f">angefordert ' + fmt(P.gReq) + ' mA</span>' : '') + '</b>' +
        '<i>Kurve</i><b>bis ' + pw.thr + ' % der Grenze (Strich) volle Helligkeit, bis zur Grenze gleitend auf ' + pw.abl + ' % (Auto-Helligkeitsbegrenzung); darüber 95 % der Grenze, höchstens ' + pw.abl + ' %</b>'
        : '<i>Gesamt</i><b>' + fmt(P.total) + ' mA <span class="np-f">ohne globale Grenze; das KO „Stromverbrauch Gesamt“ meldet 0</span></b>' +
          (ownS ? '<i>Kurve je Streifen</i><b>bis ' + ownS.thr + ' % der eigenen Grenze volle Helligkeit, bis zur Grenze gleitend auf ' + ownS.abl + ' %; darüber 95 % der Grenze, höchstens ' + ownS.abl + ' %</b>' : '')) +
      '<i>Begrenzung aktiv</i><b class="np-m">wenn der Begrenzer in den letzten 5 s mindestens ein Bild gedimmt hat</b>' +
      '<i>KOs</i><b class="np-m">Stromverbrauch Gesamt und je Streifen als DPT 7.012 (mA)</b></div></div>';
    h += '<div class="np-tw"><table class="np-tbl"><tr><th>Streifen</th><th>Anschluss</th><th>Chip</th><th>LEDs</th><th>Farbfolge</th><th>Treiber</th><th>Timing</th><th>Strom</th><th>Begrenzung</th></tr>';
    sc.strips.forEach(function (s, i) {
      var p = P.strips[i], lim = p.lim, ref = lim || (pw.mode !== 'off' ? pw.lim : 0), pct = ref ? Math.round(p.ma / ref * 100) : 0, recent = p.seen && performance.now() - p.seen < 5000;
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
    $('strips').innerHTML = h;
  }

  function renderHw() {
    var w = sc.hw, b = sc.bar, budget = b.fpsT ? 1000 / b.fpsT : 0, cpu = Math.round(w.ft / budget * 100), used = w.heapTot - w.heap, hp = Math.round(used / w.heapTot * 100), low = w.heapMin < 32;
    var bpp = { RGB: 3, RGBW: 4, RGBCCT: 5 }[b.layout] || 3, buf = sc.strips.reduce(function (a, s) { return a + s.leds; }, 0) * bpp;
    function row(l, v, wide) { return '<div class="np-r"' + (wide ? ' style="grid-column:1/-1"' : '') + '><i>' + l + '</i><b>' + v + '</b></div>'; }
    function chips(a, lab) { return '<span class="np-chips">' + a.map(function (u, i) { return '<span class="' + (u ? 'u' : '') + '">' + lab(i) + '</span>'; }).join('') + '</span>'; }
    var h = '<div class="np-grid"><div class="np-sub">Gerät</div>' + row('Prozessor', w.chip) +
      row('Hardware', w.ets ? pill('bad', 'passt nicht zur ETS') + ' gefunden ' + w.board + ' (' + w.hwid + ') · ETS erwartet ' + w.ets : w.board + ' <span class="np-f">' + w.hwid + ' · Pegelwandler ' + w.ls + '</span>', !!w.ets) +
      row('Build', w.build + (w.build === 'KNX TP + WLAN' ? ' <span class="np-f">KNX über TP, WLAN für Webseite, Einrichtung und OTA</span>' : '')) +
      row('Diese Webseite', w.web ? pill('ok', 'verfügbar') + ' <span class="np-f">Build mit Webserver</span>' : pill('idle', 'nicht in diesem Build') + ' <span class="np-f">kein Webserver; die Vorschau zeigt, was die Seite anzeigen würde</span>') +
      '<div class="np-sub">Rechenzeit</div>' +
      row('Rechenzeit pro Bild (Ø seit Start)', w.ft && budget ? meter(cpu, cpu > 85 ? 'bad' : cpu > 60 ? 'warn' : '') + w.ft + ' ms von ' + budget.toFixed(1) + ' ms (' + cpu + ' %)' : '<span class="np-f">— noch kein Bild (Startphase)</span>') +
      row('Bildrate', b.fps ? b.fps + ' fps <span class="np-f">Ziel ' + b.fpsT + '</span>' + (b.fps < b.fpsT * .8 ? ' ' + pill('warn', 'unter Ziel') : '') : '<span class="np-f">— noch kein Bild</span>') +
      row('Frame-Zeit seit Start', w.ftMax ? 'min ' + w.ftMin + ' ms · max ' + w.ftMax + ' ms' : '<span class="np-f">— noch kein Bild</span>') +
      row('CPU-Temperatur', w.temp ? w.temp.toFixed(1) + ' °C' : '<span class="np-f">— noch kein Messwert</span>') +
      '<div class="np-sub">Speicher und Treiber</div>' +
      row('Arbeitsspeicher', meter(hp, low ? 'bad' : hp > 75 ? 'warn' : '') + w.heap + ' KB frei von ' + w.heapTot + ' KB <span class="np-f">' + hp + ' % belegt</span>') +
      row('Tiefster Stand', '<span class="' + (low ? 'np-bad' : '') + '">' + w.heapMin + ' KB frei</span>' + (low ? ' ' + pill('bad', 'wenig Speicher') : '')) +
      row('LED-Puffer', fmt(buf) + ' B <span class="np-f">virtueller Streifen, ' + bpp + ' B je LED</span>') +
      (w.sm ? row('PIO-State-Machines', meter(w.sm.filter(Boolean).length / w.sm.length * 100) + w.sm.filter(Boolean).length + ' von ' + w.sm.length + ' belegt') +
        row('DMA-Kanäle', meter(w.dmaUsed / w.dma * 100) + w.dmaUsed + ' von ' + w.dma + ' belegt')
        : !w.rmt || !w.rmt.length ? row('RMT-Kanäle (Senden)', '<span class="np-f">—</span>') : row('RMT-Kanäle (Senden)', meter(w.rmt.filter(Boolean).length / w.rmt.length * 100) + w.rmt.filter(Boolean).length + ' von ' + w.rmt.length + ' belegt <span class="np-f">ein Kanal je Streifen</span>') + row('DMA', '<span class="np-f">— nicht genutzt (RMT)</span>')) +
      '<div class="np-sub">Betrieb</div>' +
      row('Laufzeit', w.up) +
      row('Startphase', w.start === 'abgeschlossen' ? pill('ok', 'abgeschlossen') : pill('warn', 'läuft') + ' ' + w.start) +
      (w.saved ? row('Zustandsdaten', w.saved + ' <span class="np-f">Zustand aus dem Flash geladen oder Farbe/Szene seit dem Start gesetzt</span>') : '') +
      row('Fehler / Warnung', w.err ? pill('bad', w.err) : pill('ok', 'keine')) + '</div>';
    $('hw').innerHTML = h;
  }

  // ---- control panel (concept: POST /neo/control {segment, function, value}) ----------------------
  // The device writes the value into the segment's input KO like a received telegram (processInputKo):
  // no bus telegram (valueNoSend + processInputKo in the loop task), lock / startup delay / Effektkette rules apply,
  // status KOs send as usual. Offered by ETS parameters, not by group address.
  // Effect names as in src/EffectIds.lock (IDs 0-19 1D, 20-32 2D)
  var FX1D = ['Solid', 'Wipe', 'Rainbow', 'Pride2015', 'Juggle', 'BPM', 'Cylon', 'Test', 'Fire', 'Theater Chase', 'Sparkle', 'Breathing', 'Strobe', 'Comet', 'Noise', 'Palette', 'Lightning', 'Gradient', 'Candle', 'Scroll Text'];
  var FX2D = ['Clock 2D', 'Snake 2D', 'Matrix 2D', 'Tetris 2D', 'TRON', 'Starfield Warp', 'Plasma Nebula', 'UFO Swarm', 'Game of Life 2D', 'DNA 2D', 'Aurora 2D', 'Lissajous 2D', 'Metaballs 2D'];
  var FXALIAS = { Wipe: 'Comet', Candle: 'Kerze', Lightning: 'Strobe', Palette: 'Rainbow', 'Theater Chase': 'Sparkle', BPM: 'Rainbow', Test: 'Rainbow', 'Scroll Text': 'Solid', 'Clock 2D': 'Solid', 'Snake 2D': 'Juggle', 'Matrix 2D': 'Aurora 2D', 'Tetris 2D': 'Juggle', TRON: 'Cylon', 'Starfield Warp': 'Sparkle', 'UFO Swarm': 'Juggle', 'Game of Life 2D': 'Sparkle', 'DNA 2D': 'Aurora 2D', 'Lissajous 2D': 'Aurora 2D', 'Metaballs 2D': 'Plasma Nebula' };
  // Offered by ETS parameters of the active segment (no group address needed):
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
      '<br><b>Ohne Anmeldung: jeder im Netz kann steuern; Anmeldung folgt mit OFM-Network.</b>' +
      '<br><span class="np-f"><b>Angeboten wird, was in der ETS für das Segment eingestellt ist</b> – auch ohne Gruppenadresse: Ein/Aus, Helligkeit, Dimmen, Sperre und Effekt für jedes aktive Segment; Farbe nach Streifentyp (RGB, RGBW, RGB + WW/CW); Szenen 1 bis „Anzahl Szenen“; Effektmanager nur, wenn er in den Basiseinstellungen verfügbar, aktiv und mit Cues angelegt ist. 2D-Effekte nur bei 2D-Segmenten.</span></div>';
    if (isPv()) h += '<div class="np-note" style="border-left-color:var(--np-pv-d);background:var(--np-pv-bg)"><b>Vorschau:</b> Die Bedienelemente wirken nur auf die Vorschau, nicht auf dieses Gerät.</div>';
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
      if (has(s, 'fx')) rows += ctlRow('Effekt', '<select class="np-sel" data-seg="' + i + '" data-f="fx">' + fxList.map(function (n) { return '<option' + (n === s.fx || FXALIAS[n] === s.fx && n === s.fxName ? ' selected' : '') + '>' + n + '</option>'; }).join('') + '</select>' + (is2D(s.geo) ? '' : '<span class="np-f">2D-Effekte nur bei 2D-Segmenten</span>'));
      if (has(s, 'scene') && s.scenes && s.scenes.length) rows += ctlRow('Szene', s.scenes.map(function (q) { return cbtn(i, 'scene', q.n, 'Szene ' + q.n, s.scene === q.n); }).join(''));
      if (ems.length) rows += ctlRow('Effektmanager', ems.map(function (e) {
        var run = s.em === e.id && /run|pause/.test(s.ems || ''), act = e.st === 1;
        return '<div class="np-ctl-em"><span><b>EM ' + e.id + '</b>' + (run ? ' · Cue ' + s.cue + '/' + e.cues.length : '') + '</span>' +
          (act ? cbtn(i, 'emStart', e.id, '▶ Start', run) + (has(s, 'emStop') ? cbtn(i, 'emStop', 1, '■ Stop', 0) : '') + (has(s, 'cue') ? cbtn(i, 'cue', -1, 'Cue −', 0) + cbtn(i, 'cue', 1, 'Cue +', 0) : '')
            : '<span class="np-f">' + (e.st === 2 ? 'suspendiert: ein Start wird ignoriert' : 'deaktiviert') + '</span>') + '</div>';
      }).join(''));
      if (has(s, 'lock')) rows += ctlRow('Sperre', cbtn(i, 'lock', s.locked ? 0 : 1, s.locked ? 'Entsperren' : 'Sperren', s.locked));
      if (!rows) rows = '<div class="np-f">Für dieses Segment ist nichts einstellbar.</div>';
      h += '<div class="np-card np-ctl"><div class="np-card-h"><h3>' + badge(SEGT(i), i + 1) + segLabel(i) + '</h3>' + stPill(s) + '<span class="np-m np-num">' + s.bri + ' %</span>' + (emOn(s) ? '<span class="np-m">EM ' + s.em + ' · Cue ' + s.cue + '</span>' : '') + (s.ch.m ? '<span class="np-m">' + (s.ch.m === 1 ? 'Master' : 'Slave') + (s.ch.override ? ' · aus der Kette gelöst' : '') + '</span>' : '') + '<span class="np-sp"></span>' + swatch(lk.c1, lk.w, lk.cw) + '</div>' +
        '<canvas class="np-stage np-ctlcv" id="ctlcv-' + i + '" aria-label="LED-Bild des Segments"></canvas><div class="np-card-b">' + rows + '<div class="np-ctl-msg" id="ctl-msg-' + i + '"></div></div></div>';
    });
    h += '<div class="np-ctl-log"><b>Letzte Befehle</b> <span class="np-f">(' + (sc.live ? 'an dieses Gerät gesendet' : 'Vorschau, nicht gesendet') + ')</span><pre id="ctl-log">' + (ctlLog.length ? esc(ctlLog.join('\n')) : '—') + '</pre></div>';
    $('ctl').innerHTML = h; ctlSz = [];
  }
  var ctlSz = [];
  function drawCtl(t) { // compact LED image per segment, same colours as the top view
    sc.segs.forEach(function (s, i) {
      var cv = $('ctlcv-' + i); if (!cv) return;
      var g = s.geo, two = is2D(g), z = ctlSz[i] || (ctlSz[i] = sizeCanvas(cv, two ? Math.min(90, 8 * g[1] + 12) : 30)), c = cv.getContext('2d'), n = s.b - s.a + 1, pad = 10;
      c.setTransform(z.dpr, 0, 0, z.dpr, 0, 0); c.fillStyle = isPv() ? '#fcfbfe' : STAGE; c.fillRect(0, 0, z.w, z.h);
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
  // Applies one command to the simulated segment like processInputKo would; returns a note or ''.
  function ctlApply(i, f, v) {
    var s = sc.segs[i], note = '';
    ctlLog.unshift('POST /neo/control ' + JSON.stringify({ segment: i + 1, function: f, value: v })); ctlLog.length = Math.min(ctlLog.length, 6);
    if (sc.sim) { sc.sim = 0; sc.px = null; note = 'Simulation angehalten, Steuerung übernimmt. '; }
    if (s.st === 'wait') return note + (f === 'dim' ? 'Startverzögerung läuft: Dimm-Telegramme werden in dieser Zeit verworfen.' : 'Startverzögerung läuft: der Befehl wird zurückgestellt und danach abgearbeitet (im Mock nicht nachgebildet).');
    if (f !== 'lock' && s.locked) return note + 'Gesperrt: der Befehl wird ignoriert.';
    if (s.ch.m === 2 && f !== 'lock') { if (f === 'emStart') note += 'Slave mit eigenem EM: Sync-Telegramme werden ignoriert, solange er läuft. '; if (s.ch.pol === 1) { s.ch.override = 1; note += 'Slave mit „Lokal hat Vorrang“: übernimmt keine Sync-Telegramme mehr bis zum Neustart. '; } else note += 'Slave mit „Sync übernimmt immer“: das nächste Sync-Telegramm überschreibt das wieder. '; }
    if (!sc.bar.power && (/scene|emStart/.test(f) || /power|bri|dim/.test(f) && +v > 0)) note += 'Das Gerät ist global aus: das Segment bleibt dunkel. ';
    // every ON path (segmentOn / segmentPowerOn): clears "EM started from off" and resumes an EM paused by OFF
    var on = function () { if (s.st !== 'lock') s.st = 'on'; s.emFromOff = 0; if (s.ems === 'pause' && s.pausedByPower) { s.ems = 'run'; s.pausedByPower = 0; note += 'EM läuft weiter. '; } }, stopEm = function () { if (emOn(s)) { var lk = lookOf(s); s.fx = lk.fx; s.c1 = lk.c1; s.w = lk.w; s.cw = lk.cw; s.ems = 'stop'; note += 'Laufender EM wurde beendet; Farbe und Effekt der Cue bleiben stehen. '; } }; // interruptEmIfRunning: EM_STOP_LEAVE
    switch (f) {
      case 'power':
        if (+v) { on(); if (!s.bri) s.bri = 100; }   // intent 0 is lifted to full on ON
        else { s.st = 'off'; if (s.ems === 'run') { s.ems = 'pause'; s.pausedByPower = 1; note += 'EM ist pausiert, solange das Segment aus ist. '; } }
        break;
      case 'bri': v = clamp(+v, 0, 100); if (!v) s.st = 'off'; else { s.bri = v; if (s.st === 'off') on(); } break;
      case 'dim':
        if (s.st === 'off') { if (+v < 0) return note + 'Segment ist aus: dunkler wird ignoriert.'; on(); s.bri = 1; break; } // heller from off ramps up from the lowest level
        s.bri = clamp(s.bri + (+v) * 4, 0, 100); break;                                                                        // the ramp stops at 0 and stays on
      case 'rgb': case 'hsv': stopEm(); s.c1 = v; s.scene = 0; break;
      case 'rgbw': stopEm(); s.c1 = String(v).split('/')[0]; s.w = clamp(+String(v).split('/')[1] || 0, 0, 255); s.scene = 0; break;
      case 'rgbw-w': stopEm(); s.w = clamp(+v, 0, 255); s.scene = 0; break;
      case 'ww': stopEm(); s.w = clamp(+v, 0, 255); s.scene = 0; break;
      case 'cw': stopEm(); s.cw = clamp(+v, 0, 255); s.scene = 0; break;
      case 'fx': stopEm(); s.scene = 0; s.fxName = v; s.fx = FX[v] ? v : FXALIAS[v] || 'Solid'; if (!FX[v]) note += 'Effektbild in der Vorschau angenähert. '; break;
      case 'scene': var q = s.scenes.filter(function (x) { return x.n === +v; })[0]; if (!q) return note + 'Szene nicht angelegt: ignoriert.'; stopEm(); s.c1 = q.c; s.bri = q.bri; s.fx = 'Solid'; s.scene = q.n; on(); break;
      case 'emStart': var e = emOf(+v); if (!e || e.st !== 1) return note + 'EM nicht startbar: ignoriert.'; var fromOff = (emOn(s) && s.emFromOff) || s.st === 'off'; if (!emOn(s)) s.pre = { fx: s.fx, c1: s.c1, w: s.w }; s.em = e.id; s.cue = 1; s.ems = 'run'; s.cueEl = -(performance.now() - T0) / 1000; s.pausedByPower = 0; on(); s.emFromOff = fromOff ? 1 : 0; if (!s.bri) s.bri = 100; if (s.ch.m) note += 'Effektkette an diesem Segment ausgesetzt, solange der EM läuft. '; break;
      case 'emStop': if (!emOn(s)) return note + 'Kein EM läuft.'; s.ems = 'stop'; s.pausedByPower = 0;
        if (s.stopRet === 'Aus' || s.emFromOff) { s.st = s.locked ? 'lock' : 'off'; note += s.emFromOff ? 'EM gestoppt; er war aus dem Aus gestartet, das Segment geht aus. ' : 'EM gestoppt, Rückkehr (Segment-Parameter): Aus. '; }
        else { if (s.pre) { s.fx = s.pre.fx; s.c1 = s.pre.c1; s.w = s.pre.w; } note += 'EM gestoppt, Rückkehr zum Zustand vor dem EM. '; }
        break;
      case 'cue': if (!emOn(s)) return note + 'Cue +/− wirkt nur, solange ein EM läuft oder pausiert.'; var e3 = emOf(s.em), nc = clamp(s.cue + (+v), 1, e3.cues.length); if (nc === s.cue) note += 'Erste bzw. letzte Cue: bleibt stehen. '; s.cue = nc; s.cueEl = -(performance.now() - T0) / 1000; break;
      case 'lock': s.locked = +v ? 1 : 0; if (s.locked && s.st !== 'off') s.st = 'lock'; else if (!s.locked && s.st === 'lock') s.st = 'on'; break;
    }
    return note;
  }
  // Live: one command to the device; its result arrives with a later /neo/state (cmds: [seq, result] of the last commands).
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
  function ctlAfter(i, note) {
    powerTick(performance.now(), curT()); renderBar(); renderPipe(); renderSegs(); renderEm(); renderChain(); renderStrips();
    if (tab === 'chain') layoutChain();
    renderCtl(); ctlMsg(i, note || 'Übernommen.');
  }
  $('ctl').addEventListener('click', function (e) {
    var b = e.target.closest('button[data-f]'); if (!b) return;
    if (sc.live) {
      var li = +b.dataset.seg;
      if (b.dataset.hold) { if (e.detail === 0) { sendCtl(li, 'dim', +b.dataset.v); setTimeout(function () { sendCtl(li, 'dim', 0); }, 250); } return; } // keyboard: short step
      var lf = b.dataset.f, lv = b.dataset.v;
      if (lf === 'fx') return;
      sendCtl(li, lf, lf === 'rgb' || lf === 'rgbw' ? lv : +lv); return;
    }
    if (b.dataset.hold) { if (e.detail === 0) { var k = +b.dataset.seg; var nt = ctlApply(k, 'dim', b.dataset.v); ctlLog.unshift('POST /neo/control ' + JSON.stringify({ segment: k + 1, function: 'dim', value: 0 })); ctlAfter(k, nt || 'Ein Schritt gedimmt (Tastatur).'); } return; } // keyboard: one step plus stop
    var i = +b.dataset.seg; ctlAfter(i, ctlApply(i, b.dataset.f, b.dataset.v));
  });
  $('ctl').addEventListener('change', function (e) {
    var el = e.target, d = el.dataset; if (!d.f) return;
    var i = +d.seg, s = sc.segs[i];
    if (sc.live) {
      if (d.f === 'rgbw' || d.f === 'rgbw-w') { var lc = d.f === 'rgbw' ? el.value : s.c1, lw = d.f === 'rgbw-w' ? +el.value : s.w || 0; sendCtl(i, 'rgbw', lc + '/' + lw); return; } // one RGBW telegram: colour + W
      if (d.f === 'fx') { var id = FX1D.concat(FX2D).indexOf(el.value); if (id >= 0) sendCtl(i, 'fx', id); return; }
      sendCtl(i, d.f, d.f === 'rgb' ? el.value : +el.value); return;
    }
    if (d.f === 'rgbw' || d.f === 'rgbw-w') { var wv = d.f === 'rgbw-w' ? +el.value : s.w || 0, cv = d.f === 'rgbw' ? el.value : s.c1; ctlAfter(i, ctlApply(i, 'rgbw', cv + '/' + wv)); return; } // one RGBW telegram: colour + W
    ctlAfter(i, ctlApply(i, d.f, d.f === 'bri' || d.f === 'ww' || d.f === 'cw' ? +el.value : el.value));
  });
  $('ctl').addEventListener('input', function (e) { // live label while dragging; the value is applied on release
    var el = e.target; if (el.dataset.f === 'bri') { var b = $('ctl-bri-' + el.dataset.seg); if (b) b.textContent = el.value + ' %'; }
  });
  (function holdDim() { // DPT 3.007: press = start, release = stop telegram
    var timer = null, seg = -1, dir = 0;
    var liveSeg = -1;
    function stop() { ctlHeld = false; if (liveSeg >= 0) { var k = liveSeg; liveSeg = -1; sendCtl(k, 'dim', 0); return; } if (timer === null) return; clearInterval(timer); timer = null; ctlLog.unshift('POST /neo/control ' + JSON.stringify({ segment: seg + 1, function: 'dim', value: 0 })); ctlLog.length = Math.min(ctlLog.length, 6); ctlAfter(seg, 'Dimmen beendet (Stopp-Telegramm).'); }
    $('ctl').addEventListener('pointerdown', function (e) {
      var b = e.target.closest('button[data-hold]'); if (!b) return;
      e.preventDefault(); seg = +b.dataset.seg; dir = +b.dataset.v;
      if (sc.live) { if (liveSeg >= 0) return; liveSeg = seg; ctlHeld = true; sendCtl(seg, 'dim', dir); return; } // press = start, release = stop telegram
      if (timer !== null) { clearInterval(timer); timer = null; }
      var note = ctlApply(seg, 'dim', dir); ctlMsg(seg, note);
      if (/ignoriert|zurückgestellt/.test(note)) { ctlAfter(seg, note); return; }
      ctlHeld = true;
      timer = setInterval(function () { var s = sc.segs[seg]; if (s.locked) return stop(); ctlApply(seg, 'dim', dir); ctlLog.shift(); var bl = $('ctl-bri-' + seg); if (bl) bl.textContent = s.bri + ' %'; powerTick(performance.now(), curT()); renderBar(); }, 120);
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
  var LSK = 'openknx-neopixel-anordnung-v1:';
  function lsLoad(k) { try { var v = localStorage.getItem(LSK + k); return v ? layFrom(JSON.parse(v)) : null; } catch (e) { return null; } }
  function lsSave(k) { try { localStorage.setItem(LSK + k, layJson()); return true; } catch (e) { return false; } }
  function lsDrop(k) { try { localStorage.removeItem(LSK + k); } catch (e) { /* storage blocked: nothing stored */ } }
  function edMsg(t) { var el = $('ed-msg'); if (el) el.textContent = t; }
  function edState() { return lay.saved ? pill('ok', 'in diesem Browser gespeichert') : pill('idle', lsHas(cur) ? 'geändert, nicht gespeichert' : 'nicht gespeichert'); }
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
    h += '</table></div><div class="np-note info">Nur eine Anzeigehilfe: Sie ändert nichts an der LED-Ausgabe; welche LED was zeigt, bestimmen weiterhin Segmente, Geometrie, Verdrahtung und Kette aus der ETS. ' +
      '<b>Gespeichert wird in diesem Browser.</b> Mit Exportieren und Importieren (JSON-Datei) nimmst du die Anordnung in einen anderen Browser oder aufs Handy mit. Später möglich: Speichern auf dem Gerät (/neo/layout.json).</div></div></div>' +
      '<div class="np-ed-f"><button class="np-btn pri" data-act="save">Im Browser speichern</button><button class="np-btn" data-act="export">Exportieren</button><button class="np-btn" data-act="import">Importieren</button><input type="file" id="ed-file" accept=".json,application/json" hidden><button class="np-btn" data-act="reset">Zurücksetzen</button>' + (lsHas(cur) ? '<button class="np-btn" data-act="drop">Gespeicherte entfernen</button>' : '') + '<span class="np-f" id="ed-msg">Änderungen siehst du sofort im LED-Bild und in der Draufsicht.</span><details><summary>Inhalt der Datei</summary><pre id="ed-json"></pre></details></div>';
    $('editor').innerHTML = h; $('ed-json').textContent = layJson();
  }
  function relayout() { layoutPlan(); renderCap(); if ($('ed-json')) $('ed-json').textContent = layJson(); }
  $('editor').addEventListener('click', function (e) {
    var b = e.target.closest('button'); if (!b) return;
    if (b.dataset.tpl) { lay.stash = lay.stash || {}; lay.stash[lay.tpl] = clone(lay.runs); lay.pstash = lay.pstash || {}; lay.pstash[lay.tpl] = clone(lay.p); lay.tpl = b.dataset.tpl; if (lay.stash[lay.tpl]) lay.runs = clone(lay.stash[lay.tpl]); lay.saved = 0; lay.p = Object.assign({}, DEF[lay.tpl], lay.tpl === sc.layout.tpl ? sc.layout.p : {}, lay.pstash[lay.tpl] || {}); lay.edge = lay.tpl === sc.layout.tpl ? sc.layout.edge : null; syncRuns(); renderEditor(); relayout(); }
    if (b.dataset.act === 'close') { $('editor').hidden = true; $('lay-edit').classList.remove('pri'); }
    if (b.dataset.act === 'reset') { lay = clone(sc.layout); renderEditor(); relayout(); edMsg('Standard-Anordnung geladen; eine gespeicherte Anordnung bleibt, bis du speicherst oder sie entfernst.'); }
    if (b.dataset.act === 'drop') { var had = lsHas(cur); lsDrop(cur); var gone = !lsHas(cur); if (lay.saved && gone) lay.saved = 0; renderEditor(); relayout(); edMsg(!had ? 'In diesem Browser ist keine Anordnung gespeichert.' : gone ? 'Gespeicherte Anordnung in diesem Browser entfernt.' : 'Entfernen nicht möglich: der Browser erlaubt hier keinen Zugriff auf den lokalen Speicher.'); }
    if (b.dataset.act === 'save') { if (lsSave(cur)) { lay.saved = 1; renderEditor(); relayout(); edMsg('In diesem Browser gespeichert.'); } else edMsg('Speichern nicht möglich: der Browser erlaubt hier keinen lokalen Speicher. Exportieren geht trotzdem.'); }
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

  // ---- live / preview + tab switching ---------------------------------------------------------------
  function setHash() { try { history.replaceState(null, '', '#' + tab + ':' + cur); } catch (e) { /* file:// in some browsers */ } }
  function renderMode() {
    var pv = isPv();
    $('neo').classList.toggle('pv', pv);
    qa('#mode button').forEach(function (b) { var on = (b.dataset.m === 'pv') === pv; b.classList.toggle('on', on); b.setAttribute('aria-pressed', on ? 'true' : 'false'); });
    $('pv').hidden = !pv; $('pvline').hidden = !pv;
    qa('#pv-tpl button, #pv-more button').forEach(function (b) { b.classList.toggle('on', b.dataset.s === cur); });
    applyUnconf();
    $('meta').innerHTML = pv ? 'Vorschau mit Beispieldaten · keine Werte dieses Geräts' : (LIVE.unconf ? 'Live-Status dieses Geräts · nicht konfiguriert' : 'Live-Status dieses Geräts · Aktualisierung jede Sekunde · zeigt, was eingestellt ist und was gerade läuft') + (LIVE.err ? ' · ' + pill(LIVE.ok ? 'warn' : 'bad', LIVE.err) : '');
    $('mocknote').hidden = true;
    if (pv) {
      $('pv-d').innerHTML = '<b>' + sc.label + '</b>' + (sc.src ? ' · Vorlage ' + sc.src : '') + ' · ' + sc.title + ' <span class="np-f">Hardware in diesem Beispiel: ' + sc.hw.board + ' (' + sc.hw.hwid + ').</span>';
      $('pvline').innerHTML = '<b>Vorschau · ' + sc.label + '</b><span>Beispieldaten, nicht dieses Gerät</span>';
    }
    $('foot').innerHTML = 'Diese Seite gibt es nur in Builds mit Webserver und Netzwerk (WLAN oder LAN), jetzt auch in den Builds KNX TP + WLAN der KNeoPiX V1 und V1.5 ESP32-S3. Die RP2040- und RP2350-Builds haben keinen Webserver.';
  }
  function render() {
    T0 = performance.now(); sc = SC[cur]; lay = lsLoad(cur); if (lay) lay.saved = 1; else lay = clone(sc.layout); plan.cube = { yaw: .6, pitch: .45, zoom: 1, auto: true }; sim = {};
    lastLimited = -1e9; power = null;
    if (sc.sim) syncSim(isNaN(tFix) ? START[cur] || 0 : tFix);
    renderMode();
    powerTick(performance.now(), isNaN(tFix) ? START[cur] || 0 : tFix); lastPw = performance.now();
    renderBar(); renderPipe(); renderSegs(); renderEm(); renderChain(); renderStrips(); renderHw(); renderEditor(); renderCtl();
    showTab(tab);
  }
  function syncSim(t) {
    garageSim(t);
    var c = sim.cue;
    sc.segs.forEach(function (s) { s.cue = c || 3; s.ems = c ? 'run' : 'end'; s.st = c ? 'on' : 'off'; });
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
  // Unconfigured device (no ETS download): only the hint, no status bar, tabs or panels; the preview stays available.
  function applyUnconf() {
    var u = isLive() && !!LIVE.unconf, U = LIVE.unconf;
    $('unconf').hidden = !u; $('bar').hidden = u; $('tabs').hidden = u;
    qa('.np-pane').forEach(function (p) { p.hidden = u || p.id !== 'p-' + tab; });
    if (u) $('unconf').innerHTML = '<b>Gerät nicht konfiguriert – bitte ETS-Applikation laden.</b><br>Ohne ETS-Download sind keine Streifen, Segmente und Effekte angelegt; diese Seite zeigt den Status, sobald die Applikation geladen ist. Die Vorschau mit Beispieldaten bleibt verfügbar.' +
      '<br><span class="np-f">' + (U.board ? esc(U.board) + ' · ' : '') + 'HW-ID ' + hex4(U.hwid || 0) + ' · Laufzeit ' + upStr(U.up || 0) + '</span>';
  }
  function chips(group) { return Object.keys(SC).filter(function (k) { return SC[k].group === group; }).map(function (k) { return '<button data-s="' + k + '">' + SC[k].label + '</button>'; }).join(''); }
  $('pv-tpl').innerHTML = chips('tpl'); $('pv-more').innerHTML = chips('more');
  // Previews always start from the example data.
  var SC0 = {}, GPX = SC.garage.px; Object.keys(SC).forEach(function (k) { SC0[k] = clone(SC[k]); });
  function restore(k) { var p0 = clone(SC0[k]); Object.keys(p0).forEach(function (key) { SC[k][key] = p0[key]; }); if (k === 'garage') SC.garage.px = GPX; }
  function pick(k) { cur = k; ctlLog = []; if (k !== 'live') { lastPv = k; restore(k); } render(); }
  $('pv').addEventListener('click', function (e) { var b = e.target.closest('button'); if (!b) return; if (b.id === 'pv-back') pick('live'); else if (b.dataset.s) pick(b.dataset.s); });
  $('mode').addEventListener('click', function (e) { var b = e.target.closest('button'); if (!b) return; var pv = b.dataset.m === 'pv'; if (pv !== isPv()) pick(pv ? lastPv : 'live'); });
  $('tabs').addEventListener('click', function (e) { var b = e.target.closest('.np-tab'); if (b) showTab(b.dataset.t); });
  window.addEventListener('resize', function () { ctlSz = []; if (tab === 'setup') layoutPlan(); if (tab === 'chain') layoutChain(); });

  var h0 = location.hash.replace('#', '').split(':');
  if (h0[1] && SC[h0[1]]) { cur = h0[1]; if (cur !== 'live') lastPv = cur; }
  if (h0[0] && $('p-' + h0[0])) tab = h0[0];
  var tFix = NaN;
  SC.live = fromState(EMPTY, []);
  render();
  livePoll();
  pixPoll();

  // ---- live polling ---------------------------------------------------------------------------------
  function liveSig(L) { return [L.segs.length, L.strips.length, L.bar.layout, L.emCount, L.chain ? L.chain.seg : -1, L.segs.map(function (s) { return s.a + '-' + s.b + ':' + s.geo.join('x'); }).join(',')].join('|'); }
  var ctlHtml = '';
  function onState(j) {
    LIVE.rxAt = performance.now(); LIVE.ok = 1; LIVE.err = '';
    if (j.unconf) { // no ETS download: hint only, nothing to show or control
      var was = LIVE.unconf; LIVE.unconf = j; LIVE.st = null; LIVE.sig = ''; LIVE.pend = {};
      if (isLive()) { if (!was) render(); else renderMode(); }
      return;
    }
    if (LIVE.unconf) { LIVE.unconf = null; LIVE.sig = ''; }
    if (LIVE.st && j.up < LIVE.st.up) { Object.keys(LIVE.pend).forEach(function (k) { ctlMsg(LIVE.pend[k].i, 'Gerät wurde neu gestartet, Ergebnis unbekannt.'); }); LIVE.pend = {}; } // device restarted: its command numbers start again
    LIVE.st = j; LIVE.tok = j.tok || '';
    if (j.emSum !== LIVE.emSum && !LIVE.emBusy) fetchEm();
    var L = fromState(j, LIVE.ems), sig = liveSig(L);
    SC.live = L;
    // command results: each command its own; one that dropped out of the device's list is unknown
    var res = {}, maxSeq = 0;
    (j.cmds || []).forEach(function (c) { res[c[0]] = c[1]; if (c[0] > maxSeq) maxSeq = c[0]; });
    var done = Object.keys(LIVE.pend).filter(function (k) { return +k <= maxSeq; });
    if (!isLive()) { done.forEach(function (k) { delete LIVE.pend[k]; }); return; }
    if (sig !== LIVE.sig) { LIVE.sig = sig; ctlHtml = ''; render(); } // configuration changed: full render, arrangement re-validated
    else {
      sc = L; power = L.power;
      renderMode(); renderBar(); renderPipe(); renderSegs(); renderEm(); renderChain(); renderStrips(); renderHw();
      if (tab === 'chain') layoutChain();
      if (tab === 'setup') renderCap();
      var foc = document.activeElement, busy = foc && $('ctl').contains(foc) && /INPUT|SELECT/.test(foc.tagName);
      if (!busy && !ctlHeld) { var keep = qa('.np-ctl-msg').map(function (m) { return [m.id, m.textContent]; }); renderCtl(); keep.forEach(function (m) { var el = $(m[0]); if (el) el.textContent = m[1]; }); }
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
    var wait = document.hidden ? 5000 : 1000;
    fetch('/neo/state', { cache: 'no-store' }).then(function (r) {
      if (r.status === 503) return { busy: 1 };
      if (!r.ok) throw new Error('HTTP ' + r.status);
      return r.json();
    }).then(function (j) {
      if (j.busy) { wait = 300; LIVE.err = 'Gerät beschäftigt, erneut versuchen …'; if (isLive()) renderMode(); return; }
      if (j.pending) { wait = 300; return; }
      onState(j);
    }).catch(function () { LIVE.ok = 0; LIVE.err = 'keine Verbindung'; if (isLive()) renderMode(); }).then(function () { setTimeout(livePoll, wait); });
  }
  function pixPoll() { // LED image while a tab with LEDs is open
    if (!isLive() || LIVE.unconf || document.hidden || !/setup|ctl|chain/.test(tab)) { setTimeout(pixPoll, 500); return; }
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
    var t = (now - T0) / 1000 + (isNaN(tFix) ? START[cur] || 0 : tFix);
    if (sc.sim) {
      var before = sim.cue + ':' + !!sim.end;
      syncSim(t);
      if (sim.cue + ':' + !!sim.end !== before) { renderSegs(); if (tab === 'em') renderEm(); var el = $('simcap'); if (el) el.innerHTML = simText(); }
    }
    if (!sc.live && now - lastPw >= 1000) { // preview: current estimate once per second (live: from /neo/state)
      lastPw = now; powerTick(now, t); renderBar();
      if (tab === 'strips') renderStrips();
      if (tab === 'em') renderEm();
    }
    if (tab === 'setup') drawPlan(t);
    else if (tab === 'chain' && sc.chain) drawChain(t);
    else if (tab === 'ctl') drawCtl(t);
  }
  requestAnimationFrame(frame);
})();
