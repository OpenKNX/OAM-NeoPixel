### LED-Typ

Wählen Sie den passenden LED-Typ für Ihren LED-Streifen aus. Verschiedene LED-Chips verwenden unterschiedliche Protokolle und Timings.

**Unterstützte LED-Typen:**

- **WS2812/WS2812B**: Standard RGB LEDs (800kHz)
- **WS2815**: 12V RGB LEDs mit Backup-Datenleitung
- **SK6812 (RGB only)**: SK6812 im 3-Kanal-Modus ohne Nutzung des Weißkanals
- **SK6812/WS2814 (RGBW)**: 4-Kanal-LEDs mit separatem Weißkanal
- **APA102/SK9822**: SPI-basierte LEDs mit separater Clock-Leitung
- **WS2805**: RGBCCT (5-Kanal) LEDs
- **SM16825**: 16-Bit High-End LEDs

**Wichtig:** Der falsche LED-Typ kann zu Flackern, falschen Farben oder gar keiner Funktion führen. Konsultieren Sie das Datenblatt Ihres LED-Streifens für den korrekten Typ.

Für einen SK6812-RGBW-Streifen muss **SK6812/WS2814 (RGBW)** gewählt werden. Die Auswahl **SK6812 (RGB only)** überträgt keinen Weißkanal.
