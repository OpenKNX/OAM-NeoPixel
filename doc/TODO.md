# OAM-NeoPixel — zentrale TODO-Liste

Einzige Quelle für offene Punkte. Detail-Pläne stehen direkt hier; abgeschlossene
Themen unten unter „Erledigt".

Regeln: alle ETS/Generator-Änderungen gehen in `scripts/Build-*.ps1` (nicht in die
generierten XML), Speicherlayout nur per Anhängen ändern, danach Producer + Release grün.

---

## 1) 2D-Effekt auf 1D-Segment → Fallback auf Solid  ·  Prio: mittel (UX/Footgun)
Wählt man einen **2D-Effekt** (z.B. Cylon 2D) für ein **1D-Segment** (keine Matrix-Topologie),
rendert die Firmware aktuell **still nichts** → Segment bleibt dunkel, kein Hinweis.

**Entscheidung:** **Firmware-Fallback** — wenn ein 2D-Effekt nicht zur 1D-Geometrie passt,
**auf „Solid" zurückfallen** (Primärfarbe anzeigen) statt dunkel zu bleiben.
- Fixt es an der Wurzel für **alle drei** Stellen (Segment, Szene, Cue) — auch den Cue,
  dessen Ziel-Topologie zur Config-Zeit unbekannt ist (EM läuft auf beliebigem Segment).
- ETS-Erkennung verworfen: beim Cue prinzipiell nicht detektierbar (Runtime-Bindung).
- Asymmetrie korrekt: **1D-Effekt auf 2D-Matrix funktioniert** (Dispatch fällt auf `update()`
  zurück, Segment.cpp ~497-505). Nur 2D-auf-1D ist das Problem.
- Ort: 2D-Effekte steigen bei `is1D()` aus bzw. ihr `update()` ist leer → statt dessen Solid
  rendern (generischer Fallback im Dispatch oder pro 2D-Effekt).

## 2) Hardware-Gating — Knoten ausblenden bis Hardware gewählt  ·  Prio: UX
Solange `NeoPixelHardwareSelect` (`%AID%_UP-4000018`) = **255** („--- Bitte wählen ---"),
alle hardwareabhängigen Top-Level-Knoten ausblenden (statt zeigen + Fehlermeldung).
- Die 5 Knoten in `NeoPixel.share.xml` (Relais, Strombegrenzung, Effektmanager, Physikalisch,
  Virtuell) in `<choose UP-4000018><when test="!=255">…</when></choose>` wrappen.
- Hinweis auf „Allgemein": „… erscheinen, sobald eine Hardware gewählt ist."
- Redundante „bitte zuerst Hardware wählen"-Fehlermeldungen danach entfernen.
- Kein Speicherlayout-Change. **Offen/Team:** Was passiert mit bereits verknüpften GAs an
  KOs ausgeblendeter Knoten? (KOs bleiben i.d.R., nur Param-Sicht weg — prüfen.)

## 3) Effektmanager / Cue — kleinere offene Punkte  ·  Prio: niedrig
- **Hue-Konvention vereinheitlichen:** ein Hue-Byte kann nicht „Segmentfarbe" UND Hue 0 (=Rot)
  kodieren. Clock2D-Regel aktuell: 0 = Segmentfarbe, >0 = fixer Hue (Rot nur via 255). Globale
  Konvention für alle Hue-Effekte festlegen + Effekt-Header/Beschreibungen angleichen.
- **Effektkette Phasen-Sync-Drift:** Slaves starten Effekte beim Sync neu, laufen dann frei →
  leichte Phasendrift (v.a. Blink/Strobe/Theater Chase). Optional: Tick-/Phasen-Zähler ins
  Sync-Telegramm (5 Byte im 14-Byte-KO frei) + Heartbeat (~30 s). Erst auf HW bewerten.
- **PT-CueEffect-Enum:** nutzt aktuell den Laufindex; optional auf explizite EffectID umstellen
  (robuster gegen Reihenfolge-Änderungen).

## 4) Überlappende Segmente — Verhalten dokumentieren  ·  Prio: niedrig
„Last writer wins" (höher nummeriertes Segment gewinnt, kein Blend) — *by design*, aber
undokumentiert. Optional kurzer ETS-Hinweis bei den Segmenten.

## 5) OFM-relevante Teile nach OFM umziehen  ·  Prio: groß, SPÄTER
Größere Aufräum-/Trennaktion: alle **OFM-/Engine-relevanten Sachen für den Producer**
sauber nach `OFM-NeoPixel` verlagern (OAM bleibt OAM, OFM bleibt OFM — keine Cross-Verweise).
Bewusst **nicht jetzt** — eigenes Vorhaben. (Erster Schritt erledigt: EM/Cue/Effektkette-
Engine-Doku liegt in `OFM-NeoPixel/doc/EM-Cue-Effektkette.md`; OFM-TODO „sound-reaktive Effekte"
in `OFM-NeoPixel/doc/TODO.md`.)

## 6) Knoten-Reihenfolge im ETS-Baum anpassen  ·  Prio: niedrig (UX, reine Kosmetik)
Workflow-gerechte Reihenfolge: **Allgemein → Physikalisch → Effektmanager → Virtuell/Segmente →
Relais → Strombegrenzung** (EM vor Virtuell, weil Segmente einen EM zuweisen). Reine UI-Knoten-
Reihenfolge in `share.xml` (kein Layout/KO). **Vorsicht:** große, teils generierte Blöcke
(Virtuell ~1000 Z., Relais in AUTO-GENERATED-Marker) — sauber per Skript + Producer-Check, nicht
Last-Minute. (Bewusst zurückgestellt.)

---

## Erledigt (Referenz)
- **Hardware-Identität build-/update-stabil über DEVICE_HW_ID** — ETS speichert die eindeutige
  Geräte-ID als Auswahlwert (statt Listenplatz), Firmware vergleicht direkt gegen `DEVICE_HW_ID`.
  Generator: Identitätsliste deterministisch nach `DEVICE_HW_ID` sortiert; **Hauptschalter**
  Dynamic/Full in `Build-Release.ps1` (Default Dynamic = nur gebaute HW). Auf ESP32-WROOM-HW getestet.
- **Drittanbieter-Warnung** — Flag `NEOPIXEL_HW_THIRDPARTY_NO_SUPPORT` → ETS-Hinweis „Hardware von
  Drittanbietern" bei deren Auswahl (Extractor → Generator → `<when>`-Block).
- **Neue Hardware** — Gledopto **GL-C-017WL** (0x2001) + **GL-C-620WL** (0x2002): ini-Sektion,
  HW-ID in `Gledopto.h`, Build-Envs, Release-Target, Pins (017WL real getestet).
- **Boot-Crash-Fix (ESP32-WROOM)** — `_emData` nicht mehr als globaler Static-Init (~76 KB `new[]`
  → bad_alloc/abort), sondern lazy in `setup()` (`std::nothrow` + Null-Guards). Dazu
  **`EM_CUE_COUNT` 99→10** (= ETS-Realität) → `_emData` ~76 KB → ~8 KB RAM.
- **Tabellenbreiten** (Segmente/Relais/Strip/Cue, Summe >100 % bis 200 %) + **„von 10"-Suffix**
  bei „Anzahl aktiver Cues".
- **Versions-Text pro Variante** — `%NEO_ProductVersion%` (Dev 0.4 / Release 0.3) statt
  `%ModuleVersion%` (Code-Version); Marketing-Text matcht jetzt die ApplicationVersion.
- **Help-Baggage bereinigt** — ~27 verwaiste `.md` entfernt (Orphans = 0); GPIO-, EM-, Cue- und
  „Verhalten nach Neustart (Segment)"-Hilfe verdrahtet.
- **Relais 1–4 als Baum-Knoten** mit editierbarem Namen (`NEO-Relais-Name`) — generiert in
  `Build-HardwareConfig.ps1`. *(Hinweis: vor Commit voll bauen, damit die Relais-UI-Marker-Region
  in share.xml befüllt ist, nicht „Cleaned".)*
- **Kontext-Hilfe EM + Cue** verdrahtet (alle `NEO-EM-*` / `NEO-Cue-*` referenziert).
- **Effekt-Defaults für Segment + Cue** — typisierte ParameterRefs mit `Value=`-Override.
- **„Effekt Typ" → „Effekt"** vereinheitlicht (Segment + Szene).
- **Master-Offset ausgeblendet** (nur Slave; Master per Definition Offset 0).
- **Szenen-/Segment-Effekt bei zugewiesenem EM ausgeblendet** + Info-Hinweis; Segmente-Tabelle
  Effekt ausgegraut (read-only) wenn EM gesetzt.
- **Topologie/Chain/Tiefe-Bilder** überarbeitet (kein Rot/Grün, S/W-Marker + Legende, Sync-Fix);
  Quellen + `regen.sh` in `doc/svg-src/`.
- **C++-Fixes:** Heap-Leak (Stack-Puffer), Cue-Index-Bounds-Guard, `_emData`-Destruktor, Flash-`reserved[]`-Doku.
