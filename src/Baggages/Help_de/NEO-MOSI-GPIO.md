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

**Reservierte Pins:** Pins, die das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion), seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, lehnt die Firmware ab; der Streifen bleibt dann aus (Log: „REFUSED“). Nutzt ein Relais oder ein vorheriger Streifen denselben GPIO schon, lehnt die Firmware den späteren Streifen ab und er bleibt aus (Log: „REFUSED“); ein SPI-Streifen belegt MOSI und Takt nur gemeinsam.

**Troubleshooting:**
Keine LED-Funktion bei SPI-LEDs deutet meist auf falsche MOSI/Clock-Pin Zuordnung hin.

