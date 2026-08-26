# Effekt-Typ

Wählt den Animationseffekt für das Segment aus.

<!-- BEGIN AUTO-GENERATED: Effect Table - DO NOT EDIT MANUALLY -->
## Verfügbare Effekte

### Standard-Effekte (immer verfügbar)

```
ID | Name                  | Beschreibung
---+-----------------------+--------------------------------------------------------
 0 | Solid                 | Statische Farbe ohne Animation
 1 | Wipe                  | Farbe läuft vom Anfang bis zum Ende über den Streifen
 2 | Rainbow               | Regenbogen über das gesamte Farbspektrum
 3 | Pride2015             | Regenbogenfarben mit wandernden Helligkeitswellen
 4 | Juggle                | Mehrere farbige Punkte pendeln hin und her und blenden aus
 5 | BPM                   | Pulsierende Farbwellen im eingestellten Takt (BPM)
 6 | Cylon                 | Klassischer KITT/Cylon-Effekt mit hin- und herspringendem Lichtauge und verblassendem Schweif. Unterstützt 1D-Streifen und 2D-Matrizen. Mode 1 erzeugt zwei gespiegelte Augen aus der Mitte (Tor)
 7 | Test                  | Kanaltest: RGBW (4 Kanäle) oder RGB+CCT (5 Kanäle)
19 | Laufschrift           | Horizontale Laufschrift auf LED-Matrix (Font 5×7/4×6/3×5, Umlaute & Ç)
```

### Erweiterte Effekte (deaktivierbar mit `NEOPIXEL_MINIMAL_EFFECTS`)

```
ID | Name                  | Beschreibung
---+-----------------------+--------------------------------------------------------
 8 | Fire                  | Realistische Feuersimulation mit Flackern
 9 | Theater Chase         | Laufende Punktmuster wie bei einer Marquee-Lichterkette
10 | Sparkle               | Zufällige Funkeln: Sparkle, Twinkle oder Confetti je nach Modus
11 | Breathing             | Sanftes Ein-/Ausatmen mit wählbarer Wellenform (weich, Halt, Puls, scharfer Puls)
12 | Strobe                | Schnelles Blitzen wie ein Stroboskop
13 | Comet                 | Bewegter Punkt mit Schweif: Comet, Meteor oder Sinelon je nach Modus
14 | Noise                 | Sanftes Rauschen entlang Streifen oder als 2D-Feld (inoise8-inspiriert)
15 | Palette               | Feste 16-Farben-Paletten, wahlweise weich überblendet und scrollend
16 | Blitz                 | Zufällige Blitzeinschläge mit kurzen Lichtblitzen und Abklingeffekt. Simuliert realistische Gewitterblitze mit einstellbarer Häufigkeit, Breite und Helligkeit
17 | Gradient              | Linearer HSV-Farbverlauf über den Streifen
18 | Kerze                 | Realistischer Kerzenflacker-Effekt. Bei Zonen=1 flackern alle LEDs gemeinsam, höhere Werte erzeugen mehrere unabhängige Flammen
```

### 2D-Effekte (erfordern Segment mit Matrix-Geometrie)

```
ID | Name                  | Beschreibung
---+-----------------------+--------------------------------------------------------
20 | Uhr 2D                | Uhr auf einer LED-Matrix: digital (HH:MM/HH:MM:SS), binär (BCD-Spalten oder Binärzeilen) oder im Wechsel. Optional mit Datumsanzeige (Wechsel oder dauerhaft, mit Wochentag). Zu breite Anzeigen scrollen automatisch bis zum Ende und zurück. Nutzt die KNX-Uhrzeit, ohne gültige Zeit läuft die Betriebszeit (Uptime)
21 | Snake 2D              | Klassisches Snake auf einer 2D LED-Matrix. Die Schlange frisst zufälliges Futter und wächst
22 | Matrix 2D             | Digitaler Matrix-Regen in 2D. Varianten: Grün, Gold oder Gemischt mit optionalen Glitch-Blitzen
23 | Tetris 2D             | Klassisches Tetris auf einer 2D LED-Matrix. Alle 7 Tetrominoes, SRS-Rotation, Geist-Stein, Linien-Flash
24 | TRON                  | Neon-Grid mit bewegten Scanlinien im TRON-Stil
25 | Starfield Warp        | Sternenfeld mit Warp-Tunnel-Effekt
26 | Plasma Nebula         | Mehrlagige Plasma-Nebel-Animation
27 | UFO Swarm             | Mehrere UFOs mit optionalem Traktorstrahl
28 | Game of Life 2D       | Conways "Spiel des Lebens" auf einer 2D LED-Matrix. Zellulärer Automat mit umlaufenden Rändern; startet zufällig neu, wenn die Population ausstirbt oder erstarrt
29 | DNA 2D                | Rotierende DNA-Doppelhelix auf einer 2D LED-Matrix: zwei gegenphasige Sinus-Stränge mit Sprossen
30 | Aurora 2D             | Polarlicht auf einer 2D LED-Matrix: weiche, fließende Lichtvorhänge aus überlagerten Sinus-Schichten
31 | Lissajous 2D          | Animierte Lissajous-Figur auf einer 2D LED-Matrix mit driftender Phase und nachglühendem Schweif
32 | Metaballs 2D          | Organische Metaballs auf einer 2D LED-Matrix: driftende Blobs, die per Feldsumme verschmelzen und sich teilen
```

**Gesamt: 33 Effekte (ID 0-32)**
<!-- END AUTO-GENERATED: Effect Table -->

## Hinweis

2D-Effekte werden auf 1D-Segmenten ignoriert. Die Matrix-Geometrie (Breite, Höhe, Topologie) muss im Abschnitt „Topologie“ konfiguriert sein.

## Weniger Effekte auf manchen Geräten

Auf Geräten mit knappem Programmspeicher liefert die Firmware die **2D-Effekte nicht mit** - sie hätten dort keinen Platz. Die ETS-Auswahl bietet deshalb von vornherein nur die Effekte an, die das Gerät wirklich darstellen kann.

Betroffen sind derzeit die **ESP32-C6-Varianten** (KNeoPix und Mini). Alle anderen Geräte zeigen die vollständige Liste.

Welche Hardware das betrifft, entsteht beim Erzeugen der Applikation automatisch aus den Firmware-Einstellungen - es muss nichts von Hand gepflegt werden.

Ein Wechsel der Hardware-Auswahl **verliert nichts**: beide Listen benutzen dieselben Werte und dasselbe Speicherbyte. Stand dort ein 2D-Effekt und du wechselst auf ein Gerät ohne 2D, verschwindet er nur aus der Auswahl - das Gerät spielt dann ersatzweise **Solid**.
