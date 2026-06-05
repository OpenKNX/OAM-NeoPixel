# Effekt-Typ

Wählt den Animationseffekt für das Segment aus.

## Verfügbare Effekte

### Standard-Effekte (immer verfügbar)

| ID | Name | Beschreibung |
|----|------|-------------|
| 0 | Solid | Statische Farbe ohne Animation |
| 1 | Wipe | Progressive Farbfüllung über den Streifen |
| 2 | Rainbow | Regenbogen-Farbverlauf |
| 3 | Rainbow Cycle | Regenbogen mit einstellbarer Dichte und Richtung |
| 4 | Pride2015 | Pride-Effekt mit Farbwellen |
| 5 | Confetti | Zufällige farbige Pixel mit Ausblendung |
| 6 | Juggle | Bewegende Sinuswellen-Punkte |
| 7 | BPM | Beat-synchrone Animation mit Farbpalette |
| 8 | Cylon | Springendes Lichtauge mit Fade-Spur (Knight Rider-Stil) |
| 9 | RGBWTest | Testeffekt für SK6812 RGBW-LEDs |
| 10 | GarageDoor | Garagentor-Animationseffekt |

### Erweiterte Effekte (deaktivierbar mit `NEOPIXEL_MINIMAL_EFFECTS`)

| ID | Name | Beschreibung |
|----|------|-------------|
| 11 | Feuer | Realistische Feuer-Simulation (FastLED Fire2012-Port) |
| 12 | Theater Chase | Klassischer Theater-Marquee-Effekt |
| 13 | Theater Chase Rainbow | Theater-Chase mit Regenbogenfarben |
| 14 | Sinelon | Bewegter Punkt mit Sinuswellen-Bewegung und Schweif |
| 15 | Twinkle | Zufällige Pixel funkeln wie Sterne |
| 16 | Sparkle | Party-Funkel-Effekt mit zufälligen Blitzen |
| 17 | Breathing | Sanfter Atem-Effekt mit konfigurierbarer Kurve |
| 18 | Strobe | Stroboskop-Effekt mit einstellbarem Timing |
| 19 | Pulse | Puls-Effekt mit konfigurierbarer Breite |
| 20 | Comet | Bewegender Komet mit verblassendem Schweif |
| 21 | Meteor | Meteor-Schauer mit zufälligen Größen und Farben |
| 22 | Noise | Sanfter Rausch-Effekt entlang des Streifens |
| 23 | Palette | Farbpaletten-Animation |
| 24 | Blitz | Blitz/Lightning-Effekt |
| 25 | Gradient | Farbverlauf-Effekt |
| 26 | RGBCCTTest | Testeffekt für 5-Kanal RGBCCT-LEDs |
| 27 | Kerze | Einzelne flackernde Kerze |
| 28 | Kerzen Multi | Mehrere unabhängig flackernde Kerzen |

### 2D-Effekte (erfordern Segment mit Matrix-Geometrie)

| ID | Name | Beschreibung |
|----|------|-------------|
| 29 | Feuer 2D | Spaltenweise Feuer-Simulation auf LED-Matrix |
| 30 | Rauschen 2D | Sanftes XY-Rauschfeld mit bilinearer Interpolation |
| 31 | Cylon 2D | Cylon-Auge als Zeile oder Spalte über die Matrix |
| 32 | Laufschrift | Horizontale Laufschrift mit 5×7 Font (Text per KO setzbar) |
| 33 | Uhr 2D | Digitale Uhr HH:MM oder HH:MM:SS (erfordert gültige KNX-Uhrzeit) |

## Hinweis
2D-Effekte werden auf 1D-Segmenten ignoriert. Die Matrix-Geometrie (Breite, Höhe, Topologie) muss im Abschnitt „Topologie" konfiguriert sein.

**Gesamt: 34 Effekte (ID 0–33)**