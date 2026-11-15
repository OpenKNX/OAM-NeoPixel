### Clock GPIO

Der GPIO-Pin für das Clock-Signal bei SPI-basierten LEDs (APA102, APA102-Klon, SK9822, WS2801, LPD8806, LPD6803, P9813). Nur erforderlich wenn SPI-LEDs verwendet werden.

**Konfiguration:**

- Für PWM-LEDs (WS2812, SK6812): Dieser Pin wird ignoriert
- Für SPI-LEDs (APA102, SK9822): Zwingend erforderlich
- Geben Sie die GPIO-Nummer ein (z.B. 18 für GPIO18)

**RP2040 SPI-Pins:**

- **SPI0**: Clock auf GPIO 2, 6, 18, 22
- **SPI1**: Clock auf GPIO 10, 14, 26
- Das Modul wählt automatisch den passenden SPI-Controller

**ESP32 SPI-Pins:**

- **HSPI**: Clock meist auf GPIO 14
- **VSPI**: Clock meist auf GPIO 18
- Andere GPIO-Pins via Software-SPI möglich

**Wichtig:**

- Clock und Daten-GPIO müssen zum gleichen SPI-Controller gehören
- Ohne Auswahl des Clock-Ports bleibt der SPI-Streifen aus; „Manuell“ verwendet den Clock-GPIO aus dem Feld darunter
- Ohne passende Hardware-Auswahl auf der Seite OpenKNX → „Gerät“ (keine oder eine andere Hardware als die der Firmware) nutzt das Gerät keinen Port und auch keinen manuellen GPIO: der Streifen bleibt aus
- Belegen zwei Einstellungen denselben GPIO der gewählten Hardware (Port, „Manuell“ oder ein fest verdrahtetes Relais), erscheint bei jeder eine Meldung „GPIO-Konflikt“; ohne gewählte Hardware wird nicht geprüft
- Kurze, gleichlange Leitungen für Clock und Daten verwenden
- Clock-Signal ist kritisch für SPI-LED Funktion
- Pins, die das Gerät für KNX (TX/RX, nur bei einer TP-Firmware), den Save-Pin, seine Taster (Prog, Funktion), seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C5 16–18 und 20–22, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, lehnt die Firmware ab; der Streifen bleibt dann aus (Log: „REFUSED“); ein solcher GPIO wird bei der Eingabe gemeldet („GPIO reserviert“)
- Nutzt ein Relais oder ein vorheriger Streifen denselben GPIO schon, lehnt die Firmware den späteren Streifen ab und er bleibt aus (Log: „REFUSED“); ein SPI-Streifen belegt MOSI und Takt nur gemeinsam.

