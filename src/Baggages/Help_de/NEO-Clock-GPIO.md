### Clock GPIO

Der GPIO-Pin für das Clock-Signal bei SPI-basierten LEDs (APA102, SK9822). Nur erforderlich wenn SPI-LEDs verwendet werden.

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
- Kurze, gleichlange Leitungen für Clock und Daten verwenden
- Clock-Signal ist kritisch für SPI-LED Funktion

