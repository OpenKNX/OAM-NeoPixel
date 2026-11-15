# Effekt-Flags

Effekte lassen sich einzeln oder in Gruppen aus dem Build nehmen, um Flash zu sparen.
Gesetzt werden die Flags als `build_flags` im jeweiligen Env in `platformio.custom.ini`.

Diese Liste wird von `scripts/Build-EffectParameters.ps1` aus den Effekt-Headern erzeugt.

<!-- BEGIN AUTO-GENERATED: Effect Flags - DO NOT EDIT MANUALLY -->
Alle 33 Effekte sind im Build, solange kein Flag sie herausnimmt.
Solid ist der Rueckfall fuer eine unbekannte Auswahl und laesst sich nicht abschalten.

## Sammelschalter

```
  -D NEOPIXEL_DISABLE_2D          alle 13 2D-Effekte
  -D NEOPIXEL_MINIMAL_EFFECTS     die 11 erweiterten Effekte
```

## Einzeln

### Immer verfuegbar (8)

```
  -D NEOPIXEL_DISABLE_WIPE            Wipe
  -D NEOPIXEL_DISABLE_RAINBOW         Rainbow
  -D NEOPIXEL_DISABLE_PRIDE           Pride2015
  -D NEOPIXEL_DISABLE_JUGGLE          Juggle
  -D NEOPIXEL_DISABLE_BPM             BPM
  -D NEOPIXEL_DISABLE_CYLON           Cylon
  -D NEOPIXEL_DISABLE_RGBWTEST        Test
  -D NEOPIXEL_DISABLE_SCROLLTEXT      Laufschrift
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

### 2D (13) - alle von NEOPIXEL_DISABLE_2D abgedeckt

```
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

## Hinweise

- Ein abgeschalteter Effekt verschwindet auch aus der ETS-Auswahl, sofern die Hardware
  ueber `NEOPIXEL_DISABLE_2D` gebaut wird. Bei einzelnen Flags bleibt der Eintrag in ETS
  sichtbar; das Geraet faellt dann auf **Solid** zurueck.
- Solid laesst sich nicht abschalten, es ist der Rueckfall fuer eine unbekannte Auswahl.
