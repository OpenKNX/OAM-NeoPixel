### Daten GPIO

Der GPIO-Pin, der für die Datenübertragung zu den LEDs verwendet wird. Bei PWM-basierten LEDs (WS2812, SK6812) wird nur ein Datenpin benötigt.

**Konfiguration:**

- Geben Sie die GPIO-Nummer ein (z.B. 16 für GPIO16)
- Der Pin muss als Ausgang konfigurierbar sein
- Vermeiden Sie Pins, die für andere Funktionen reserviert sind

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

