#pragma once

static const char kNeoPixelStudioHtml[] = R"HTML(
<div class="container" id="neopixel-studio" data-active-colour="primary">
<div class="studio-title"><h1>NeoPixel Studio</h1><p id="studio-status" role="status" aria-live="polite">Connecting…</p></div>
<noscript>JavaScript is required to control NeoPixel Studio.</noscript>
<form id="studio-form" hidden>
<div class="studio-toolbar">
<div class="studio-segment-select"><label for="studio-segment">Segment</label><select id="studio-segment"></select><span class="meta" id="studio-segment-state"></span></div>
<div class="studio-actions"><button type="button" id="studio-auto-apply" aria-pressed="true">Auto apply: On</button><button type="button" id="studio-save">Save to device</button><details class="studio-more"><summary>Actions</summary><div><button type="button" id="studio-apply">Apply live</button><button type="button" id="studio-reset">Reset draft</button><button type="button" id="studio-load">Reload live values</button><button type="button" id="studio-restore-ets">Restore ETS settings</button></div></details></div>
</div>
<div class="studio-tabs" role="tablist" aria-label="Studio pages"><button type="button" id="studio-tab-control" role="tab" aria-selected="true" aria-controls="studio-panel-control">Steuern</button><button type="button" id="studio-tab-scenes" role="tab" aria-selected="false" aria-controls="studio-panel-scenes" tabindex="-1">Szenen</button><button type="button" id="studio-tab-commission" role="tab" aria-selected="false" aria-controls="studio-panel-commission" tabindex="-1">Inbetriebnahme</button></div>
<fieldset id="studio-controls"><legend class="studio-sr">Live controls</legend>
<section id="studio-panel-control" role="tabpanel" aria-labelledby="studio-tab-control">
<div class="studio-live-grid"><div class="studio-card"><div class="studio-card-heading"><h2>Live output</h2><label class="studio-power-label" for="studio-power">Power <input type="checkbox" id="studio-power"></label></div>
<div class="studio-row"><label for="studio-effect">Effect</label><select id="studio-effect"></select></div>
<div class="studio-row"><label for="studio-brightness">Brightness</label><input id="studio-brightness" type="range" min="0" max="255"><output id="studio-brightness-value" for="studio-brightness"></output></div>
<div class="studio-colour-select" role="group" aria-label="Colour to edit"><button type="button" data-colour="primary" aria-pressed="true">Primary colour</button><button type="button" data-colour="secondary" aria-pressed="false">Secondary colour</button></div><div id="studio-colours"></div></div>
<div class="studio-card"><div class="studio-card-heading"><h2>Effect parameters</h2></div><div id="studio-parameters"></div></div></div>
</section>
<section id="studio-panel-scenes" role="tabpanel" aria-labelledby="studio-tab-scenes" hidden><div class="studio-card"><div class="studio-card-heading"><h2>Scenes</h2></div><div id="studio-scenes"></div></div></section>
</fieldset>
<section id="studio-panel-commission" role="tabpanel" aria-labelledby="studio-tab-commission" hidden>
<div class="studio-live-grid"><div class="studio-card"><div class="studio-card-heading"><h2>Verkabelung testen</h2><span class="studio-badge" id="studio-test-status">Bereit</span></div><p class="meta studio-note">Testet das ausgewählte Segment mit der aktuell heruntergeladenen Aufteilung. Globale Abschaltung, Sperren und Stromgrenzen gelten weiter. Stop stellt die vorherigen Werte wieder her; automatisch nach 60 Sekunden.</p><label class="studio-test-level" for="studio-test-level">Test-Helligkeit <input id="studio-test-level" type="range" min="1" max="64" value="32"><output id="studio-test-level-value">13%</output></label>
<div class="studio-test-buttons"><button type="button" data-test="red">Rot</button><button type="button" data-test="green">Grün</button><button type="button" data-test="blue">Blau</button><button type="button" data-test="white">Weiß</button><button type="button" data-test="warm">Warmweiß</button><button type="button" data-test="cool">Kaltweiß</button><button type="button" data-test="chase">Lauflicht</button><button type="button" data-test="cycle">Kanalwechsel</button></div><p class="meta">RGB: Weißtöne angenähert. RGBW: WW/KW testen denselben Weißkanal. RGBCCT: getrennte WW/KW-Kanäle. HCL ist im Testsegment vorübergehend ausgesetzt.</p><button type="button" id="studio-test-stop">■ Stop · vorherigen Zustand wiederherstellen</button></div>
<div class="studio-card"><div class="studio-card-heading"><h2>LED-Konfiguration</h2><span class="studio-badge">Live + ETS</span></div>
<p class="meta studio-note">LED-Nummern beginnen bei 1. Auto apply wendet Änderungen direkt an. Für den dauerhaften Startzustand speichern, in ETS übernehmen und herunterladen. GPIO, LED-Typ und Stromgrenzen werden in ETS konfiguriert.</p>
<fieldset id="studio-commission-fields"><legend class="studio-sr">LED configuration draft</legend><div id="studio-strip-fields"></div><h3>Segmentbereich im virtuellen Streifen</h3><div class="studio-inline-fields"><label for="studio-start-led">Erste LED<input id="studio-start-led" type="number" min="1" max="16383" step="1" required></label><label for="studio-end-led">Letzte LED<input id="studio-end-led" type="number" min="1" max="16383" step="1" required></label></div><button type="button" id="studio-commission-apply">LED-Konfiguration live anwenden</button></fieldset><p class="meta" id="studio-commission-status"></p></div>
</div>
</section>
<details class="studio-help"><summary>Pending changes &amp; help <span id="studio-draft"></span></summary><pre id="studio-payload"></pre><p class="meta">Auto apply updates live output. Save persists the selected segment's visual settings, scenes and commissioning draft for ETS import. Reload reads live values; Restore ETS settings reapplies downloaded visual settings. ETS remains the configuration source.</p></details>
</form></div>
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
#neopixel-studio .studio-picker-tabs button[aria-pressed=true] {background:#e1efdc;border-color:#449841;color:#1c391c}
#neopixel-studio input[type=checkbox] {appearance:none;-webkit-appearance:none;width:34px;height:20px;border-radius:20px;background:#c9cfca;position:relative;cursor:pointer;flex-shrink:0}
#neopixel-studio input[type=checkbox]:before {content:"";position:absolute;left:3px;top:3px;width:14px;height:14px;border-radius:50%;background:#fff;transition:transform .15s}
#neopixel-studio input[type=checkbox]:checked {background:#449841}
#neopixel-studio input[type=checkbox]:checked:before {transform:translateX(14px)}
#neopixel-studio #studio-colours {align-items:start}
#neopixel-studio .studio-picker[data-mode=mixed] .studio-white-preview {height:32px}
#neopixel-studio[data-ready=false] .studio-title {flex-direction:column;align-items:flex-start;gap:8px}
#neopixel-studio[data-ready=false] #studio-status {max-width:100%;text-align:left;font-size:13px}
#neopixel-studio .studio-inline-fields label {display:flex;flex-direction:column;justify-content:space-between}
/* Explicit tracks avoid native padding and thumb-centre endpoint gaps. */
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone) {appearance:none;-webkit-appearance:none;padding:0;border:0;background:transparent;cursor:pointer}
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone)::-webkit-slider-runnable-track {height:6px;border:0;border-radius:3px;background:linear-gradient(to right,#449841 0%,#449841 var(--studio-fill,0%),#e0e4df var(--studio-fill,0%),#e0e4df 100%)}
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone)::-webkit-slider-thumb {appearance:none;-webkit-appearance:none;width:16px;height:16px;margin-top:-5px;border:0;border-radius:50%;background:#449841}
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone)::-moz-range-track {height:6px;border:0;border-radius:3px;background:linear-gradient(to right,#449841 0%,#449841 var(--studio-fill,0%),#e0e4df var(--studio-fill,0%),#e0e4df 100%)}
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone)::-moz-range-progress {background:transparent}
#neopixel-studio input[type=range]:not(.studio-hue):not(.studio-white-tone)::-moz-range-thumb {width:16px;height:16px;border:0;border-radius:50%;background:#449841}
/* Studio-scoped overrides: shared OpenKNX pages keep their existing layout. */
body:has(#neopixel-studio) main {padding:20px 24px;min-width:0;flex:1}
body:has(#neopixel-studio) nav {width:190px;min-width:190px}
#neopixel-studio {max-width:1680px;margin:0 auto;width:100%;font-size:14px}
#neopixel-studio [hidden] {display:none!important}
#neopixel-studio h1 {font-size:22px;margin:0;line-height:1.3}
#neopixel-studio h2 {font-size:15px;margin:0;text-transform:none;letter-spacing:0;color:#18221b}
#neopixel-studio h3 {font-size:13px;margin:12px 0 6px}
#neopixel-studio .studio-title {display:flex;gap:16px;align-items:baseline;justify-content:space-between;margin-bottom:12px}
#neopixel-studio #studio-status {margin:0;font-size:12px;text-align:right;max-width:65%}
#neopixel-studio button,#neopixel-studio select,#neopixel-studio input:not([type=range]):not([type=checkbox]) {font:inherit;border:1px solid #cbd2cc;border-radius:6px;background:#fff;color:#202720;padding:7px 10px;margin:0;min-height:34px}
#neopixel-studio button {cursor:pointer;white-space:normal;line-height:1.25;-webkit-appearance:none}
#neopixel-studio button:hover {background:#f2f6f1;border-color:#82a67a}
#neopixel-studio button:disabled {opacity:.45;cursor:default}
#neopixel-studio button[aria-pressed=true] {background:#e1efdc;border-color:#449841;color:#1c391c}
#neopixel-studio #studio-save,#neopixel-studio #studio-commission-apply {background:#316c2d;color:white;border-color:#316c2d}
#neopixel-studio .studio-toolbar {display:flex;gap:10px;align-items:center;flex-wrap:wrap;background:#f5f7f4;border-left:3px solid #449841;padding:9px 12px;margin-bottom:14px}
#neopixel-studio .studio-segment-select {display:flex;gap:8px;align-items:center;flex:1;flex-wrap:wrap}
#neopixel-studio #studio-segment-state {font-size:12px;margin:0}
#neopixel-studio .studio-actions {margin:0;gap:6px;align-items:center}
#neopixel-studio .studio-more {position:relative}
#neopixel-studio summary {cursor:pointer;font-size:12px}
#neopixel-studio .studio-more summary {border:1px solid #cbd2cc;padding:8px 10px;border-radius:6px;background:white;font-size:13px}
#neopixel-studio .studio-more>div {position:absolute;right:0;top:40px;display:grid;gap:6px;padding:10px;border:1px solid #ccc;border-radius:8px;background:white;box-shadow:0 5px 16px #0002;min-width:190px;z-index:4}
#neopixel-studio .studio-tabs {display:flex;border-bottom:1px solid #cbd2cc;gap:4px;margin-bottom:14px;overflow-x:auto}
#neopixel-studio .studio-tabs button {border:1px solid transparent;border-radius:6px 6px 0 0;background:transparent;flex-shrink:0;padding:9px 15px}
#neopixel-studio .studio-tabs button[aria-selected=true] {border-color:#cbd2cc;border-bottom-color:white;background:white;font-weight:600}
#neopixel-studio .studio-live-grid {display:grid;grid-template-columns:minmax(0,1.55fr) minmax(280px,1fr);gap:16px;align-items:start}
#neopixel-studio .studio-card {border:1px solid #d5dbd4;border-radius:8px;padding:0 14px 14px;min-width:0;background:#fff}
#neopixel-studio .studio-card-heading {display:flex;align-items:center;justify-content:space-between;gap:8px;padding:11px 0;border-bottom:1px solid #e6e9e4;margin-bottom:6px}
#neopixel-studio .studio-power-label {display:flex;align-items:center;gap:8px;font-size:13px}
#neopixel-studio .studio-row {min-height:42px;padding:8px 0;gap:10px}
#neopixel-studio .studio-row label {width:110px;flex-shrink:0;font-size:13px}
#neopixel-studio .studio-row select {flex:1;width:100%}
#neopixel-studio input[type=range] {height:20px;max-width:none;accent-color:#449841;margin:0}
#neopixel-studio #studio-colours {gap:12px;margin-top:10px}
#neopixel-studio .studio-picker {padding:10px;border-radius:7px}
#neopixel-studio .studio-picker-heading {margin-bottom:8px}
#neopixel-studio .studio-picker h3 {margin:0}
#neopixel-studio .studio-picker-tabs {margin-bottom:10px}
#neopixel-studio .studio-picker-tabs button {border-radius:0;min-height:32px;padding:5px 8px}
#neopixel-studio .studio-picker-tabs button:first-child {border-radius:6px 0 0 6px}
#neopixel-studio .studio-picker-tabs button:last-child {border-radius:0 6px 6px 0}
#neopixel-studio .studio-spectrum,#neopixel-studio .studio-white-preview {height:125px;border-radius:4px}
#neopixel-studio .studio-picker-label {font-size:12px;margin:9px 0 5px}
#neopixel-studio .studio-hex-row {margin-top:9px}
#neopixel-studio .studio-rgb-numbers {margin-top:8px;gap:6px}
#neopixel-studio .studio-white-help,#neopixel-studio .studio-mixed {margin-top:8px;font-size:12px}
#neopixel-studio .studio-colour-select {display:none}
#neopixel-studio .studio-note {font-size:12px;line-height:1.5;margin:10px 0}
#neopixel-studio .studio-badge {font-size:11px;border-radius:20px;background:#e3efdf;color:#315a2a;padding:3px 8px;white-space:nowrap}
#neopixel-studio .studio-inline-fields {display:flex;gap:10px;margin:9px 0 12px}
#neopixel-studio .studio-inline-fields label {flex:1;font-size:12px;min-width:0}
#neopixel-studio .studio-inline-fields input {display:block;width:100%;margin-top:5px!important}
#neopixel-studio .studio-test-buttons {display:grid;grid-template-columns:repeat(4,1fr);gap:8px;margin:12px 0}
#neopixel-studio [data-test=red] {border-bottom:3px solid #d84949}
#neopixel-studio [data-test=green] {border-bottom:3px solid #449841}
#neopixel-studio [data-test=blue] {border-bottom:3px solid #477dc2}
#neopixel-studio .studio-test-level {display:flex;align-items:center;gap:10px;font-size:13px}
#neopixel-studio .studio-test-level input {flex:1;min-width:40px}
#neopixel-studio #studio-test-stop {width:100%}
#neopixel-studio .studio-help {margin:14px 0;border-top:1px solid #eee;padding-top:10px}
#neopixel-studio #studio-draft {font-size:11px;color:#687365;margin-left:8px}
@media(max-width:1100px) {
#neopixel-studio .studio-live-grid {grid-template-columns:1fr}
}
@media(max-width:640px) {
body:has(#neopixel-studio) {padding-bottom:30px}
body:has(#neopixel-studio) main {padding:12px}
body:has(#neopixel-studio) nav {display:grid;grid-template-columns:64px minmax(0,1fr);align-items:center;width:100%;min-width:0;padding:4px 8px}
body:has(#neopixel-studio) .logo {width:54px;padding:0;margin:0}
body:has(#neopixel-studio) .logo img {display:block;max-height:42px}
body:has(#neopixel-studio) .menu-container {flex-wrap:nowrap;justify-content:flex-start;overflow-x:auto;margin:0;min-width:0}
body:has(#neopixel-studio) nav a.menu {font-size:12px;padding:10px 9px;white-space:nowrap;flex-shrink:0;border-left:0;border-bottom:2px solid transparent}
body:has(#neopixel-studio) nav a.menu.active {border-bottom-color:#449841}
body:has(#neopixel-studio) .nav-wiki {display:none}
body:has(#neopixel-studio) .nav-footer {font-size:10px;white-space:nowrap;flex-wrap:nowrap;justify-content:center;padding:6px 8px;gap:10px;max-height:28px;overflow:hidden}
body:has(#neopixel-studio) .nav-footer .nav-firm,body:has(#neopixel-studio) .nav-footer .prog-status {display:none}
#neopixel-studio h1 {font-size:19px}
#neopixel-studio .studio-title {align-items:center;gap:8px;margin-bottom:9px}
#neopixel-studio #studio-status {font-size:11px;max-width:52%}
#neopixel-studio .studio-toolbar {padding:8px;margin-bottom:10px;gap:8px}
#neopixel-studio .studio-segment-select {flex-basis:100%}
#neopixel-studio .studio-actions {width:100%;display:grid;grid-template-columns:1.2fr 1.2fr .8fr}
#neopixel-studio .studio-actions button,#neopixel-studio .studio-more summary {font-size:12px;text-align:center;min-height:36px}
#neopixel-studio .studio-tabs {margin-bottom:10px;gap:0}
#neopixel-studio .studio-tabs button {padding:9px 13px}
#neopixel-studio .studio-card {padding:0 10px 10px}
#neopixel-studio .studio-row {flex-wrap:nowrap;gap:8px}
#neopixel-studio .studio-row label {width:85px}
#neopixel-studio .studio-numeric {width:62px;padding:5px}
#neopixel-studio input:not([type=range]):not([type=checkbox]),#neopixel-studio select {font-size:16px}
#neopixel-studio .studio-colour-select {display:flex;margin-top:10px}
#neopixel-studio .studio-colour-select button {flex:1;font-size:12px}
#neopixel-studio #studio-colours {display:block;margin-top:8px}
#neopixel-studio[data-active-colour=primary] .studio-picker[data-key=secondary],#neopixel-studio[data-active-colour=secondary] .studio-picker[data-key=primary] {display:none}
#neopixel-studio .studio-spectrum,#neopixel-studio .studio-white-preview {height:110px}
#neopixel-studio .studio-test-buttons {grid-template-columns:repeat(2,1fr)}
#neopixel-studio #studio-draft {display:block;margin:4px 0 0}
}
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
    function levelPercent(value) {
        const percent = Number(value) * 100 / 255, rounded = Math.round(percent);
        return (percent > 0 && percent < 100 && (rounded === 0 || rounded === 100) ? percent.toFixed(1) : rounded) + '%';
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
        el('controls').disabled = busy || !segment || segment.locked || segment.testing || segment.detailsLoaded === false;
        el('commission-fields').disabled = busy || !segment || segment.locked || segment.testing || segment.detailsLoaded === false;
        root.querySelectorAll('[data-test]').forEach(b => { b.disabled = busy || !segment || segment.locked || segment.em || !state.globalPower || segment.detailsLoaded === false || (b.dataset.test === 'chase' && !effects.some(e => e.id === 1)) || (b.dataset.test === 'cycle' && !effects.some(e => e.id === 7)); });
        el('test-stop').disabled = busy || !segment?.testing;
        el('test-level').disabled = busy || !segment || segment.testing;
        el('segment').disabled = busy || state?.segments.some(s => s.testing);
        el('save').disabled = busy || !segment || segment.testing;
        el('load').disabled = busy || !segment;
        el('restore-ets').disabled = busy || !segment || segment.locked || segment.detailsLoaded === false;
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
        panel.className = 'studio-picker'; panel.dataset.key = key;
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
            panel.dataset.mode = memory.mode;
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
            find('-level-value').textContent = levelPercent(memory.level);
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
        root.dataset.ready = String(state.ready);
        const segment = current();
        if (!state.ready) {
            el('form').hidden = true;
            status(state.programming ? 'ETS download in progress. Studio will be available after the device restarts.'
                : state.configured === false ? 'This device has not been programmed with ETS. Download the ETS application to configure its segments before using NeoPixel Studio.'
                : 'Waiting for the device to finish startup…');
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
        el('brightness-value').textContent = levelPercent(segment.brightness);
        for (const key of ['primary', 'secondary']) pickers.get(key)?.sync(segment[key], force);
        for (const p of segment.parameters) set('param-' + p.id, p.value, p.type === 'bool');
        renderScenes(segment);
        renderCommissioning(segment);
        root.querySelectorAll('input[type=range]').forEach(paintRange);
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
        if (state) next.segments = next.segments.map(segment => dirty.has(segment.id) ? { ...state.segments.find(s => s.id === segment.id), locked:segment.locked, em:segment.em, commissioning:segment.commissioning, commissioningPending:segment.commissioningPending, commissioningApplied:segment.commissioningApplied, testing:segment.testing } : segment);
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
                    if (!next.error && (action === 'batch' || action === 'load' || action === 'restoreEts' || action === 'testStart' || action === 'testStop')) { dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); if (action === 'load' || action === 'restoreEts' || action === 'testStart' || action === 'testStop') pickerMemory.clear(); signature = ''; }
                    accept(next);
                    if (next.completedId === queued.id && next.error) throw new Error(next.error);
                    const message = action === 'save' ? 'Saved to device · import this segment’s saved Studio settings in ETS' : action === 'testStart' ? 'Test aktiv · Stop oder automatische Rückkehr nach 60 Sekunden' : action === 'testStop' ? 'Test beendet · vorheriger Zustand wiederhergestellt' : action === 'commissioning' ? 'LED-Konfiguration live angewendet · Save to device für ETS-Import drücken' : action === 'load' ? 'Reloaded current live values' : action === 'restoreEts' ? 'Restored downloaded ETS settings · saved Studio snapshot unchanged' : 'Applied';
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
    el('auto-apply').addEventListener('click', async () => {
        autoApply = !autoApply;
        el('auto-apply').setAttribute('aria-pressed', String(autoApply));
        el('auto-apply').textContent = 'Auto apply: ' + (autoApply ? 'On' : 'Off');
        if (autoApply && !busy) { if (!await stageCommissioning()) return; if (dirty.has(selected)) await sendCommand('batch', buildBatch()); }
        else if (!busy) status(autoApply ? 'Automatic apply enabled' : 'Automatic apply disabled · edit a draft, then Apply live');
    });
    el('apply').addEventListener('click', async () => { if (await stageCommissioning()) await sendCommand('batch', buildBatch()); });
    el('reset').addEventListener('click', () => {
        dirty.delete(selected); visualDirty.delete(selected); sceneActions.delete(selected); pickerMemory.clear(); signature = '';
        state.segments = state.segments.map(s => s.id === selected ? clone(liveState.segments.find(live => live.id === selected)) : s); render(true); status('Draft reset');
    });
    el('save').addEventListener('click', async () => {
        if (!await stageCommissioning()) return;
        if (dirty.has(selected) && !await sendCommand('batch', buildBatch())) return;
        sendCommand('save', null);
    });
    el('load').addEventListener('click', () => sendCommand('load', null));
    el('restore-ets').addEventListener('click', () => sendCommand('restoreEts', null));
    el('form').addEventListener('submit', event => event.preventDefault());
    el('segment').addEventListener('change', () => { selected = Number(el('segment').value); signature = ''; render(); clearTimeout(timer); poll(); });
    el('effect').addEventListener('change', () => command('effect', Number(el('effect').value)));
    el('brightness').addEventListener('input', () => { el('brightness-value').textContent = levelPercent(el('brightness').value); });
    el('power').addEventListener('change', () => command('power', Number(el('power').checked)));
    el('brightness').addEventListener('change', () => command('brightness', Number(el('brightness').value)));
    const commissioningDrafts = new Map(), commissioningDirty = new Set(), commissioningCorrections = new Map();
    let commissioningSignature = '', commissioningTimer;
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
                const entry = document.createElement('div'); entry.className = 'studio-strip';
                entry.innerHTML = `<h3>Streifen ${strip.id}</h3><div class="studio-inline-fields"><label for="studio-strip-${strip.id}-length">Anzahl LEDs / Gruppen<input id="studio-strip-${strip.id}-length" type="number" min="0" max="16383" step="1" required></label><label for="studio-strip-${strip.id}-skip">Erste LEDs überspringen<input id="studio-strip-${strip.id}-skip" type="number" min="0" max="255" step="1" required></label></div>`;
                el('strip-fields').append(entry);
                for (const key of ['length','skip']) el(`strip-${strip.id}-${key}`).addEventListener('input', event => {
                    const current = commissioningDrafts.get(selected);
                    const oldTotal = current.strips.reduce((sum,s) => sum+s.length,0);
                    current.strips.find(s => s.id === strip.id)[key] = Number(event.target.value);
                    const total = current.strips.reduce((sum,s) => sum+s.length,0);
                    if (key === 'length' && total > 0) {
                        if (current.end === oldTotal || current.end > total) current.end = Math.min(16383,total);
                        current.start = Math.min(current.start,current.end);
                        el('start-led').value = current.start; el('end-led').value = current.end;
                    }
                    commissioningDirty.add(selected); el('commission-status').textContent = 'Ungespeicherter Entwurf · mit Save to device für ETS sichern.'; clampCommissioningEnd(); scheduleCommissioning();
                });
                for (const key of ['length','skip']) el(`strip-${strip.id}-${key}`).addEventListener('change', () => scheduleCommissioning(0));
            }
        }
        for (const strip of draft.strips) for (const key of ['length','skip']) {
            const input = el(`strip-${strip.id}-${key}`);
            if (input !== document.activeElement) input.value = strip[key];
        }
        const rangeMaximum = Math.min(16383, draft.strips.reduce((sum,s) => sum+s.length,0));
        el('end-led').max = Math.max(1, rangeMaximum);
        for (const key of ['start','end']) if (el(key + '-led') !== document.activeElement) el(key + '-led').value = draft[key];
        if (!commissioningDirty.has(selected)) el('commission-status').textContent = segment.commissioningApplied === false ? 'Gespeicherter Entwurf · mit LED-Konfiguration live anwenden aktivieren.' : segment.commissioningPending ? 'Live-Konfiguration weicht von ETS ab · für dauerhaften Startzustand speichern, in ETS importieren und herunterladen.' : 'Aktuelle ETS-Aufteilung · Auto apply wendet Änderungen direkt an.';
        el('test-status').textContent = segment.testing ? 'Test aktiv · max. 60 s' : segment.locked ? 'KNX-Sperre aktiv' : !state.globalPower ? 'Global ausgeschaltet' : segment.em ? 'Effektmanager läuft' : 'Bereit';
        root.querySelectorAll('[data-test="warm"],[data-test="cool"]').forEach(b => { b.hidden = false; });
    }
    function clampCommissioningEnd() {
        const draft = commissioningDrafts.get(selected);
        const maximum = Math.min(16383, draft.strips.reduce((sum,s) => sum+s.length,0));
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
        if (invalid) { invalid.reportValidity(); status('Ungültige LED-Konfiguration · Eingabebereich prüfen.', true); return false; }
        const draft = commissioningDrafts.get(selected);
        if (draft.strips.some(s => s.length ? s.skip >= s.length : s.skip !== 0) || draft.start > draft.end || draft.end > draft.strips.reduce((sum,s) => sum+s.length,0)) {
            status('LED-Bereich und übersprungene LEDs müssen innerhalb der Streifen liegen.', true); return false;
        }
        const segmentId = selected;
        const submitted = JSON.stringify(draft);
        const result = await sendCommand('commissioning', clone(draft));
        if (result && JSON.stringify(commissioningDrafts.get(segmentId)) === submitted) { commissioningDirty.delete(segmentId); if (selected === segmentId) renderCommissioning(liveState.segments.find(s => s.id === segmentId)); }
        if (result) commissioningCorrections.delete(segmentId);
        if (clampedMaximum !== null && result) { const message = `Letzte LED auf ${clampedMaximum} begrenzt (Summe der LED-Anzahlen) · live angewendet.`; status(message); el('commission-status').textContent = message; }
        return result;
    }
    for (const key of ['start','end']) el(key + '-led').addEventListener('input', event => {
        commissioningDrafts.get(selected)[key] = Number(event.target.value); commissioningDirty.add(selected);
        el('commission-status').textContent = 'Ungespeicherter Entwurf · mit Save to device für ETS sichern.';
        clampCommissioningEnd(); scheduleCommissioning();
    });
    for (const key of ['start','end']) el(key + '-led').addEventListener('change', () => { if (!autoApply) clampCommissioningEnd(); scheduleCommissioning(0); });
    el('commission-apply').addEventListener('click', () => stageCommissioning(true));
    root.querySelectorAll('[data-test]').forEach(button => button.addEventListener('click', () => sendCommand('testStart', {pattern:button.dataset.test,level:Number(el('test-level').value)})));
    el('test-stop').addEventListener('click', () => sendCommand('testStop', null));
    el('test-level').addEventListener('input', () => { el('test-level-value').textContent = levelPercent(el('test-level').value); });
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
)JS";
