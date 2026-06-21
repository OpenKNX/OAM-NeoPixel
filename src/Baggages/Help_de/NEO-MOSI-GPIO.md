### MOSI GPIO

Der GPIO-Pin für das MOSI-Signal (Master Out Slave In) bei SPI-basierten LEDs. MOSI überträgt die Farbdaten an die LEDs.

**Anwendung:**

- Nur für SPI-LEDs wie APA102, SK9822 erforderlich
- Für PWM-LEDs (WS2812, SK6812) wird dieser Pin ignoriert
- Arbeitet zusammen mit dem Clock-GPIO

**RP2040 SPI-Pins:**

- **SPI0**: MOSI auf GPIO 3, 7, 19, 23
- **SPI1**: MOSI auf GPIO 11, 15, 27
- Clock und MOSI müssen zum gleichen SPI-Controller gehören

**ESP32 SPI-Pins:**

- **HSPI**: MOSI meist auf GPIO 13
- **VSPI**: MOSI meist auf GPIO 23
- Andere Pins via Software-SPI verfügbar

**Verkabelung:**

- MOSI des Controllers → DI (Data In) der ersten LED
- Clock des Controllers → CI (Clock In) der ersten LED
- Verwenden Sie geschirmte Kabel bei langen Verbindungen

**Troubleshooting:**
Keine LED-Funktion bei SPI-LEDs deutet meist auf falsche MOSI/Clock-Pin Zuordnung hin.

