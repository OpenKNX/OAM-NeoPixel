# Effekt

Wählt den Effekt dieser Szene. Zusätzlich bietet die Auswahl „Effektmanager n starten“ (Werte 200 bis 215).

Nach der Auswahl erscheinen darunter die Einstellungen des gewählten Effekts. Effekte ohne Einstellungen (z. B. Solid) zeigen keine weiteren Felder.

## Effekte

Die Auswahl zeigt die Effekte in dieser Reihenfolge, 1D/2D- und 2D-Effekte mit vorangestelltem „1D/2D ·“ bzw. „2D ·“. Die Nummer ist der Wert des Effekts am Kommunikationsobjekt.

### 1D: für Streifen

Auf einem Segment mit Matrix-Geometrie laufen sie entlang der LED-Reihenfolge.

* **Solid** (Nr. 0) - Statische Farbe ohne Animation
* **Wipe** (Nr. 1) - Farbe läuft vom Anfang bis zum Ende über den Streifen
* **Rainbow** (Nr. 2) - Regenbogen über das gesamte Farbspektrum
* **Pride2015** (Nr. 3) - Regenbogenfarben mit wandernden Helligkeitswellen
* **Juggle** (Nr. 4) - Mehrere farbige Punkte pendeln hin und her und blenden aus
* **BPM** (Nr. 5) - Pulsierende Farbwellen im eingestellten Takt (BPM)
* **Test** (Nr. 7) - Kanaltest: RGBW (4 Kanäle) oder RGB+CCT (5 Kanäle)
* **Theater Chase** (Nr. 9) - Laufende Punktmuster wie bei einer Marquee-Lichterkette
* **Sparkle** (Nr. 10) - Zufällige Funkeln: Sparkle, Twinkle oder Confetti je nach Modus
* **Breathing** (Nr. 11) - Sanftes Ein-/Ausatmen mit wählbarer Wellenform (weich, Halt, Puls, scharfer Puls)
* **Strobe** (Nr. 12) - Schnelles Blitzen wie ein Stroboskop
* **Comet** (Nr. 13) - Bewegter Punkt mit Schweif: Comet, Meteor oder Sinelon je nach Modus
* **Palette** (Nr. 15) - Feste 16-Farben-Paletten, wahlweise weich überblendet und scrollend
* **Blitz** (Nr. 16) - Zufällige Blitzeinschläge mit kurzen Lichtblitzen und Abklingeffekt. Simuliert realistische Gewitterblitze mit einstellbarer Häufigkeit, Breite und Helligkeit
* **Gradient** (Nr. 17) - Linearer HSV-Farbverlauf über den Streifen
* **Kerze** (Nr. 18) - Realistischer Kerzenflacker-Effekt. Bei Zonen=1 flackern alle LEDs gemeinsam, höhere Werte erzeugen mehrere unabhängige Flammen

### 1D/2D: für Streifen und Matrix

* **Cylon** (Nr. 6) - Klassischer KITT/Cylon-Effekt mit hin- und herspringendem Lichtauge und verblassendem Schweif. Unterstützt 1D-Streifen und 2D-Matrizen. Mode 1 erzeugt zwei gespiegelte Augen aus der Mitte (Tor)
* **Fire** (Nr. 8) - Realistische Feuersimulation mit Flackern
* **Noise** (Nr. 14) - Sanftes Rauschen entlang Streifen oder als 2D-Feld (inoise8-inspiriert)

### 2D: nur für Matrix

Diese Effekte brauchen ein Segment mit Matrix-Geometrie (Seite „Aufbau“, Abschnitt „Topologie“).

* **Laufschrift** (Nr. 19) - Horizontale Laufschrift auf LED-Matrix (Font 5×7/4×6/3×5, Umlaute & Ç)
* **Uhr** (Nr. 20) - Uhr auf einer LED-Matrix: digital (HH:MM/HH:MM:SS), binär (BCD-Spalten oder Binärzeilen) oder im Wechsel. Optional mit Datumsanzeige (Wechsel oder dauerhaft, mit Wochentag). Zu breite Anzeigen scrollen automatisch bis zum Ende und zurück. Nutzt die KNX-Uhrzeit, ohne gültige Zeit läuft die Betriebszeit (Uptime)
* **Snake** (Nr. 21) - Klassisches Snake auf einer 2D LED-Matrix. Die Schlange frisst zufälliges Futter und wächst
* **Matrix** (Nr. 22) - Digitaler Matrix-Regen in 2D. Varianten: Grün, Gold oder Gemischt mit optionalen Glitch-Blitzen
* **Tetris** (Nr. 23) - Klassisches Tetris auf einer 2D LED-Matrix. Alle 7 Tetrominoes, SRS-Rotation, Geist-Stein, Linien-Flash
* **TRON** (Nr. 24) - Neon-Grid mit bewegten Scanlinien im TRON-Stil
* **Starfield Warp** (Nr. 25) - Sternenfeld mit Warp-Tunnel-Effekt
* **Plasma Nebula** (Nr. 26) - Mehrlagige Plasma-Nebel-Animation
* **UFO Swarm** (Nr. 27) - Mehrere UFOs mit optionalem Traktorstrahl
* **Game of Life** (Nr. 28) - Conways "Spiel des Lebens" auf einer 2D LED-Matrix. Zellulärer Automat mit umlaufenden Rändern; startet zufällig neu, wenn die Population ausstirbt oder erstarrt
* **DNA** (Nr. 29) - Rotierende DNA-Doppelhelix auf einer 2D LED-Matrix: zwei gegenphasige Sinus-Stränge mit Sprossen
* **Aurora** (Nr. 30) - Polarlicht auf einer 2D LED-Matrix: weiche, fließende Lichtvorhänge aus überlagerten Sinus-Schichten
* **Lissajous** (Nr. 31) - Animierte Lissajous-Figur auf einer 2D LED-Matrix mit driftender Phase und nachglühendem Schweif
* **Metaballs** (Nr. 32) - Organische Metaballs auf einer 2D LED-Matrix: driftende Blobs, die per Feldsumme verschmelzen und sich teilen

## Geräte ohne 2D-Effekte

Ist die Firmware des gewählten Geräts ohne 2D-Effekte gebaut, bietet die Auswahl nur die 1D- und 1D/2D-Effekte an. Diese Auswahl führt einen eigenen Wert: Nach einem Wechsel des Gerätetyps den Effekt prüfen. Ein eingestellter Effekt, den die Firmware nicht enthält, läuft als „Solid“; über das Kommunikationsobjekt wird eine solche Nummer ignoriert.
