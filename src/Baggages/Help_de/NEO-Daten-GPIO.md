### Daten GPIO

Der GPIO-Pin, der für die Datenübertragung zu den LEDs verwendet wird. Bei PWM-basierten LEDs (WS2812, SK6812) wird nur ein Datenpin benötigt.

**Konfiguration:**

- Geben Sie die GPIO-Nummer ein (z.B. 16 für GPIO16)
- Der Pin muss als Ausgang konfigurierbar sein
- Ohne Auswahl („Wählen Sie bitte ein Port aus“) bleibt der Streifen aus; „Manuell“ verwendet den GPIO aus dem Feld darunter
- Vermeiden Sie Pins, die für andere Funktionen reserviert sind
- Pins, die das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion), seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, lehnt die Firmware ab: der Streifen bleibt dann aus (Log: „REFUSED“)
- Nutzt ein Relais oder ein vorheriger Streifen denselben GPIO schon, lehnt die Firmware den späteren Streifen ab und er bleibt aus (Log: „REFUSED“); ein SPI-Streifen belegt MOSI und Takt nur gemeinsam.

**RP2040 Hinweise:**

- Fast alle GPIO-Pins können verwendet werden (0-29)
- Einige Pins können für andere Board-Funktionen reserviert sein

**ESP32 Hinweise:**

- Verwenden Sie bevorzugt GPIO 12-27 für LED-Daten
- Vermeiden Sie GPIO 0, 2, 4, 5 (Boot-Pins)
- GPIO 34-39 sind nur Eingänge

**Tipps:**

- Verwenden Sie kurze Leitungen für bessere Signalqualität
- Bei langen Leitungen kann ein Signalverstärker nötig sein

