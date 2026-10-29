#pragma once

#include "buildtime.h"

#define NEOPIXEL_STUDIO_STR2(x) #x
#define NEOPIXEL_STUDIO_STR(x) NEOPIXEL_STUDIO_STR2(x)
#define NEOPIXEL_STUDIO_VERSION NEOPIXEL_STUDIO_STR(BUILD_TIMESTAMP)

/** @brief Studio page body; style and script are gzip web assets (web/assets/studio.css, studio.js), the build timestamp busts their cache. */
static const char kNeoPixelStudioHtml[] = "<link rel='stylesheet' href='/assets/np-base.css?v=" NEOPIXEL_STUDIO_VERSION "'><link rel='stylesheet' href='/assets/studio.css?v=" NEOPIXEL_STUDIO_VERSION "'>" R"HTML(
<div class="container" id="neopixel-studio" data-active-colour="primary">
<h1>NeoPixel Studio</h1>
<div class="np-note bad" id="studio-unconf" hidden></div>
<div class="np-bar" id="studio-bar">
<span class="np-b st-seg"><span>Segment</span><span class="st-sp"><button type="button" class="st-sp-btn" id="studio-segment" aria-haspopup="listbox" aria-expanded="false" title="Segment wählen"><b class="np-badge t1" id="studio-b-num">1</b><i aria-hidden="true">▾</i></button><span class="st-sp-pop" id="studio-seglist" role="listbox" aria-label="Segmente" hidden></span></span></span>
<span class="np-b st-seg"><span>Zustand</span><span class="np-pill idle" id="studio-state"><i class="np-dot"></i><span></span></span></span>
<span class="np-b st-seg st-hs"><span>Effekt</span><b id="studio-b-effect">–</b></span>
<span class="np-b st-seg"><span>Helligkeit</span><b id="studio-b-bri">–</b></span>
<span class="np-b st-seg st-hs"><span>Szene</span><b id="studio-b-scene">–</b></span>
<span class="np-b st-seg" id="studio-b-em" hidden><span>Effektmanager</span><span class="np-pill info" title="Eine Farb- oder Effektänderung übernimmt die Steuerung"><i class="np-dot"></i>läuft</span></span>
<span class="np-b st-seg"><span>Modus</span><button type="button" class="np-pill ok" id="studio-mode" title="Umschalten zwischen Live und Entwurf"><i class="np-dot"></i><span>Live</span></button></span>
<span class="np-b notes"><span>Hinweise</span><span class="np-pill info st-hint" id="studio-status" role="status" aria-live="polite"><i class="np-dot"></i><span>Verbinde …</span></span></span>
</div>
<noscript>NeoPixel Studio benötigt JavaScript.</noscript>
<form id="studio-form" hidden>
<div class="st-row">
<div class="np-tabs" role="tablist" aria-label="Studio-Seiten"><button type="button" class="np-tab" id="studio-tab-control" role="tab" aria-selected="true" aria-controls="studio-panel-control">Steuern</button><button type="button" class="np-tab" id="studio-tab-scenes" role="tab" aria-selected="false" aria-controls="studio-panel-scenes" tabindex="-1">Szenen</button><button type="button" class="np-tab" id="studio-tab-commission" role="tab" aria-selected="false" aria-controls="studio-panel-commission" tabindex="-1">Inbetriebnahme</button></div>
<div class="st-acts"><button type="button" class="np-btn" id="studio-apply" hidden>Übernehmen</button><button type="button" class="np-btn pri" id="studio-save">Im Gerät speichern</button><details class="st-menu" id="studio-more"><summary aria-label="Weitere Aktionen">⋯</summary><div>
<label class="st-mi" for="studio-auto-apply"><span>Änderungen sofort live<small>Aus: erst sammeln, dann übernehmen</small></span><input type="checkbox" id="studio-auto-apply" checked></label><hr>
<button type="button" class="st-mi" id="studio-load"><span>Live-Werte neu laden<small>Aktuelle Werte vom Gerät lesen</small></span></button>
<button type="button" class="st-mi" id="studio-restore-ets"><span>Gerätekonfiguration wiederherstellen<small>Darstellung auf die geladene Konfiguration zurücksetzen</small></span></button><hr>
<p>„Im Gerät speichern“ legt Darstellung, Szenen und LED-Konfiguration dieses Segments als Startzustand im Gerät ab (wirkt bei Startverhalten „Letzter Zustand“). Die Gerätekonfiguration bleibt führend und kann den Stand übernehmen.</p>
</div></details></div>
</div>
<fieldset id="studio-controls"><legend class="studio-sr">Live-Steuerung</legend>
<section id="studio-panel-control" role="tabpanel" aria-labelledby="studio-tab-control">
<div class="studio-live-grid"><div class="np-card"><div class="np-card-h"><h3>Ausgabe</h3><span class="np-sp"></span><label class="studio-power-label" for="studio-power">Ein/Aus <input type="checkbox" id="studio-power"></label></div><div class="np-card-b">
<div class="studio-row"><label for="studio-effect">Effekt</label><select id="studio-effect"></select></div>
<div class="studio-row"><label for="studio-brightness">Helligkeit</label><input id="studio-brightness" type="range" min="0" max="255"><output id="studio-brightness-value" for="studio-brightness"></output></div>
<div class="studio-colour-select" role="group" aria-label="Zu bearbeitende Farbe"><button type="button" data-colour="primary" aria-pressed="true">Farbe 1</button><button type="button" data-colour="secondary" aria-pressed="false">Farbe 2</button></div><div id="studio-colours"></div></div></div>
<div class="np-card"><div class="np-card-h"><h3>Effektparameter</h3></div><div class="np-card-b" id="studio-parameters"></div></div></div>
</section>
<section id="studio-panel-scenes" role="tabpanel" aria-labelledby="studio-tab-scenes" hidden><div class="np-card"><div class="np-card-h"><h3>Szenen</h3></div><div class="np-card-b" id="studio-scenes"></div></div></section>
</fieldset>
<section id="studio-panel-commission" role="tabpanel" aria-labelledby="studio-tab-commission" hidden>
<div class="studio-live-grid"><div class="np-card"><div class="np-card-h"><h3>Verkabelung testen</h3><span class="np-sp"></span><span class="np-pill ok" id="studio-test-status">Bereit</span></div><div class="np-card-b"><p class="np-note">Testet das ausgewählte Segment mit der aktuell geladenen Aufteilung. Globale Abschaltung, Sperren und Stromgrenzen gelten weiter. Muster und Test-Helligkeit lassen sich während des Tests wechseln. Der Test läuft, bis Stop gedrückt wird, und solange diese Seite offen ist; danach werden die vorherigen Werte wiederhergestellt.</p><label class="studio-test-level" for="studio-test-level">Test-Helligkeit <input id="studio-test-level" type="range" min="1" max="64" value="32"><output id="studio-test-level-value">13 %</output></label>
<div class="studio-test-buttons"><button type="button" data-test="red">Rot</button><button type="button" data-test="green">Grün</button><button type="button" data-test="blue">Blau</button><button type="button" data-test="white">Weiß</button><button type="button" data-test="warm">Warmweiß</button><button type="button" data-test="cool">Kaltweiß</button><button type="button" data-test="chase">Lauflicht</button><button type="button" data-test="cycle">Kanalwechsel</button></div><p class="np-note">RGB: Weißtöne angenähert. RGBW: Warm- und Kaltweiß testen denselben Weißkanal. RGBCCT: getrennte Warm- und Kaltweißkanäle. HCL ist im Testsegment vorübergehend ausgesetzt.</p><button type="button" id="studio-test-stop">■ Stop · vorherigen Zustand wiederherstellen</button></div></div>
<div class="np-card"><div class="np-card-h"><h3>LED-Konfiguration</h3><span class="np-sp"></span><span class="np-pill idle">Live</span></div><div class="np-card-b">
<p class="np-note">LED-Nummern beginnen bei 1. Im Modus Live gelten Änderungen sofort. Für den dauerhaften Startzustand im Gerät speichern und in die Gerätekonfiguration übernehmen. GPIO, LED-Typ und Stromgrenzen legt die Gerätekonfiguration fest.</p>
<fieldset id="studio-commission-fields"><legend class="studio-sr">Entwurf der LED-Konfiguration</legend><div class="st-strip st-head"><span>Streifen</span><span>Anzahl LEDs / Gruppen</span><span>Erste LEDs überspringen</span></div><div id="studio-strip-fields"></div><div class="st-strip st-head"><span>Segmentbereich im virtuellen Streifen</span><span>Erste LED</span><span>Letzte LED</span></div><div class="st-strip"><span class="st-sname"><b class="np-badge t1" id="studio-c-seg">1</b><span id="studio-c-segname">Segment 1</span></span><input id="studio-start-led" type="number" min="1" max="16383" step="1" required aria-label="Erste LED"><input id="studio-end-led" type="number" min="1" max="16383" step="1" required aria-label="Letzte LED"></div><div class="st-cfoot"><p class="np-note" id="studio-commission-status"></p><button type="button" class="np-btn" id="studio-commission-apply">Live anwenden</button></div></fieldset></div></div>
</div>
</section>
<section class="np-card st-draft" id="studio-draft" hidden><div class="np-card-h"><h3>Entwurf</h3><span class="np-pill warn">noch nicht am Gerät</span><span class="np-sp"></span><button type="button" class="np-btn" id="studio-reset">Verwerfen</button></div><div class="np-card-b"><ul id="studio-payload"></ul></div></section>
</form></div>
<script defer src="/assets/studio.js?v=)HTML" NEOPIXEL_STUDIO_VERSION R"HTML("></script>
)HTML";
