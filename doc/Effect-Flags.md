# Effekt-Flags

Effekte lassen sich einzeln oder in Gruppen aus dem Build nehmen, um Flash zu sparen.
Gesetzt werden die Flags als `build_flags` im jeweiligen Env in `platformio.custom.ini`.

Diese Liste wird von `scripts/Build-EffectParameters.ps1` aus den Effekt-Headern erzeugt.

<!-- BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY -->
Alle 33 Effekte sind im Build, solange kein Flag sie herausnimmt.
Solid ist der Rueckfall fuer eine unbekannte Auswahl und laesst sich nicht abschalten.

## Sammelschalter

```
  -D NEOPIXEL_DISABLE_2D          alle 14 2D-Effekte
  -D NEOPIXEL_MINIMAL_EFFECTS     die 11 erweiterten Effekte
```

## Einzeln

### Immer verfuegbar (7)

```
  -D NEOPIXEL_DISABLE_WIPE            Wipe
  -D NEOPIXEL_DISABLE_RAINBOW         Rainbow
  -D NEOPIXEL_DISABLE_PRIDE           Pride2015
  -D NEOPIXEL_DISABLE_JUGGLE          Juggle
  -D NEOPIXEL_DISABLE_BPM             BPM
  -D NEOPIXEL_DISABLE_CYLON           Cylon
  -D NEOPIXEL_DISABLE_RGBWTEST        Test
```

### Erweitert (11) - alle von NEOPIXEL_MINIMAL_EFFECTS abgedeckt

```
  -D NEOPIXEL_DISABLE_FIRE            Fire
  -D NEOPIXEL_DISABLE_THEATERCHASE    Theater Chase
  -D NEOPIXEL_DISABLE_SPARKLE         Sparkle
  -D NEOPIXEL_DISABLE_BREATHING       Breathing
  -D NEOPIXEL_DISABLE_STROBE          Strobe
  -D NEOPIXEL_DISABLE_COMET           Comet
  -D NEOPIXEL_DISABLE_NOISE           Noise
  -D NEOPIXEL_DISABLE_PALETTE         Palette
  -D NEOPIXEL_DISABLE_LIGHTNING       Blitz
  -D NEOPIXEL_DISABLE_GRADIENT        Gradient
  -D NEOPIXEL_DISABLE_CANDLE          Kerze
```

### 2D (14) - alle von NEOPIXEL_DISABLE_2D abgedeckt

```
  -D NEOPIXEL_DISABLE_SCROLLTEXT      Laufschrift
  -D NEOPIXEL_DISABLE_CLOCK2D         Uhr 2D
  -D NEOPIXEL_DISABLE_SNAKE2D         Snake 2D
  -D NEOPIXEL_DISABLE_MATRIX2D        Matrix 2D
  -D NEOPIXEL_DISABLE_TETRIS2D        Tetris 2D
  -D NEOPIXEL_DISABLE_TRON2D          TRON
  -D NEOPIXEL_DISABLE_STARFIELDWARP2D  Starfield Warp
  -D NEOPIXEL_DISABLE_PLASMANEBULA2D  Plasma Nebula
  -D NEOPIXEL_DISABLE_UFOSWARM2D      UFO Swarm
  -D NEOPIXEL_DISABLE_GAMEOFLIFE2D    Game of Life 2D
  -D NEOPIXEL_DISABLE_DNA2D           DNA 2D
  -D NEOPIXEL_DISABLE_AURORA2D        Aurora 2D
  -D NEOPIXEL_DISABLE_LISSAJOUS2D     Lissajous 2D
  -D NEOPIXEL_DISABLE_METABALLS2D     Metaballs 2D
```
<!-- END AUTO-GENERATED: Effect Flags -->

## Eigene Gruppen

Gruppen der Applikation stehen in `src/NeoPixel.EffectGroups.xml`:

```xml
<EffectGroups>
  <Group name="Klein" text="..." effects="FireEffect CandleEffect" />
</EffectGroups>
```

- `name`: Buchstaben, Ziffern, `_`; das Flag ist `NEOPIXEL_DISABLE_GROUP_<NAME>` (Grossbuchstaben).
- `effects`: Klassennamen wie in `src/EffectIds.lock`. Ein unbekannter Name, Solid, ein doppelter
  Eintrag oder ein doppelter Gruppenname brechen `scripts/Build-EffectParameters.ps1` ab.
- `-D NEOPIXEL_DISABLE_GROUP_<NAME>` im Env nimmt jedes Mitglied heraus.
- Die Sammelschalter oben kommen aus OFM-NeoPixel: `NEOPIXEL_DISABLE_2D` (`EffectPool.h`) und
  `NEOPIXEL_MINIMAL_EFFECTS` (`EffectPool.cpp`). Der Generator liest die Zugehoerigkeit aus den
  Guards von `EffectPool::getEffectByIndex()`.

## Generierter Header

`scripts/Build-EffectParameters.ps1` schreibt `src/NeoPixelEffectGroups.h`:

- je Gruppe: `#ifdef NEOPIXEL_DISABLE_GROUP_<NAME>` setzt die `NEOPIXEL_DISABLE_<EFFEKT>`-Flags der
  Mitglieder,
- je Effektnummer n: `NEO_FXOFF_<n>` = 1, wenn der Effekt nicht im Build ist (eigenes Flag,
  `NEOPIXEL_MINIMAL_EFFECTS` fuer die erweiterten, `NEOPIXEL_DISABLE_2D` fuer die 2D-Effekte, jede
  Gruppe), sonst 0.

`platformio.custom.ini` bindet den Header in jede Uebersetzungseinheit ein
(`-include $PROJECT_DIR/src/NeoPixelEffectGroups.h` in `[custom] build_flags`, alle Envs), also vor
`EffectPool.h`. `-Clean` loescht ihn nicht.

## Verhalten

- Ein abgeschalteter Effekt behaelt seine Effektnummer; keine andere Nummer verschiebt sich.
  Segment, Szene und Cue mit einer abgeschalteten Nummer laufen als **Solid**.
- Ein Build mit `NEOPIXEL_MINIMAL_EFFECTS` linkt (die generierte Effekt-Zuordnung
  `src/EffectTypeMapping.h` beachtet das Flag).
- Auf Hardware, die ueber `NEOPIXEL_DISABLE_2D` gebaut wird, zeigt ETS eine Auswahl ohne die
  2D-Effekte. Die uebrigen Effekte bleiben in der Auswahl.
- ETS prueft den gewaehlten Effekt von Segment, Szene und Cue gegen die Firmware der gewaehlten
  Hardware (`HardwareProfile.xml <BoardTable>` ueber `NEO_FXOFF_<n>` mit den Build-Flags jedes Envs):
  „Effekt nicht verfügbar" (Fehler), wenn jede Firmware-Variante des Boards ihn weglaesst; ein Hinweis,
  wenn nur eine Variante ihn weglaesst; keine Meldung ohne oder mit unbekannter Hardware.
- Solid laesst sich nicht abschalten, es ist der Rueckfall fuer eine unbekannte Auswahl.

## Erzeugte Ausgaben (für Entwickler)

`scripts/Build-EffectParameters.ps1` erzeugt aus den Effekt-Headern von OFM-NeoPixel die Effekt-Teile der ETS-XML
(Segment, Szene, Cue, Effektlisten), `src/EffectParameterMapping.h`, `src/EffectTypeMapping.h`,
`src/NeoPixelEffectGroups.h`, den Effektblock von `src/NeoPixel.script.src.js`, den Web-Katalog
(`web/assets/effects.json`, `src/WebUi/NeoPixelEffectCatalog.generated.h`), die Hilfedateien der Effekt-Parameter
(`src/Baggages/Help_de/NEO-<Parameter>-<Effekt>.md`) und die erzeugten Blöcke von `NEO-Effekt-Typ.md`,
`NEO-Effekt-ID-Referenz.md` und dieser Seite.

- **Committet:** alle Ausgaben liegen im Repo; ein Build ohne Effektänderung erzeugt nichts.
- **`src/EffectParameters.fingerprint`:** ein Hash über die Eingaben (Effekt-Quellen von OFM-NeoPixel,
  `src/EffectIds.lock`, `src/EffectParameters.lock`, `src/NeoPixel.EffectGroups.xml`, das Skript selbst, die Sprache)
  und je Ausgabe ein Inhalts-Hash (bei Dateien mit erzeugten Blöcken nur über diese Blöcke; eine Handänderung außerhalb
  zählt nicht). Stimmen Eingaben und Ausgaben, meldet der Lauf „Effekte: unverändert" und schreibt nichts.
- **`-Check`:** schreibt nichts; Exit 0 = aktuell, 1 = Eingabe oder Ausgabe geändert oder Ausgabe fehlt (für CI).
- **`-Force`:** erzeugt auch bei unverändertem Fingerabdruck; mit `-Clean` ohne Rückfrage.
- **`-Clean`:** löscht die erzeugten Dateien, leert die Markerblöcke und löscht den Fingerabdruck; der nächste Lauf
  erzeugt alles neu. `src/NeoPixelEffectGroups.h` und die beiden Lock-Dateien bleiben.
- **Hilfedateien:** jeder volle Lauf schreibt sie aus dem Effekt-Header (`getParameterDescription()`); eine Handänderung
  wird beim nächsten Lauf überschrieben. Eine Hilfedatei, die der vorige Lauf erzeugt hat und dieser nicht mehr (z. B.
  nach dem Entfernen eines Parameters), wird gelöscht. Fehlt in einer anderen gelisteten Ausgabe der Markerblock, bricht
  der Lauf ab und `-Check` bleibt „veraltet".
- **`scripts/Build-Release.ps1 -Clean`** lässt die Effekt-Ausgaben und den Fingerabdruck stehen.

### Lock-Dateien

- **`src/EffectIds.lock`:** Effektnummer je Effektklasse. Neue Effekte werden angehängt; eine geänderte Nummer oder ein
  aus `getEffectByIndex()` entfernter Effekt bricht ab.
- **`src/EffectParameters.lock`:** je Effekt Index, Name und ETS-Typ jedes Parameters (`bool`, `string`,
  `enum:<Wert>=<Text>;…`, `number:<min>..<max>`, `color:<min>..<max>`). Die ETS-IDs (`21CC<EE><PP>`, `22CC<Cue><EE><P>`)
  hängen am Index, also behält ein Parameter Index und Typ. Erlaubt: anhängen, umbenennen bei gleichem Typ, den letzten
  Parameter entfernen (der Index bleibt als „removed" belegt), Enum-Werte anhängen, einen Zahlenbereich erweitern.
  Abbruch: einen Parameter verschieben (einfügen, umsortieren, mittendrin entfernen), einen entfernten Index neu
  vergeben, einen anderen Typ, einen geänderten oder entfernten Enum-Wert, einen engeren Zahlenbereich (wie die ETS beim
  „Aktualisieren" mit einem gespeicherten Wert außerhalb des neuen Bereichs umgeht, ist nicht geprüft; der Generator
  verlässt sich nicht darauf). Ausweg: den Parameter lassen und einen neuen anhängen.
- Beide Locks schreibt nur ein vollständig erfolgreicher Lauf, zusammen mit dem Fingerabdruck; ein abgebrochener Lauf und
  `-WebCatalogOnly` lassen sie unverändert. Fehlt `src/EffectParameters.lock`, legt der Lauf sie aus dem aktuellen Stand
  an und meldet das (erster Lauf, nichts verglichen). Beide Dateien werden committet.
- Regressionstest: `pwsh test/Test-EffectChanges.ps1` spielt die erlaubten und verbotenen Änderungen in einer Kopie durch.
