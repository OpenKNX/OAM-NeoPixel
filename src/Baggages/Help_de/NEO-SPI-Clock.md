### SPI Clock

Die Taktfrequenz für SPI-basierte LED-Chips wie APA102 oder SK9822. Diese LEDs benötigen ein separates Clock-Signal zusätzlich zur Datenleitung.

**Verfügbare Geschwindigkeiten:**
- **1 MHz**: Niedrigste Geschwindigkeit, beste Kompatibilität
- **2 MHz**: Standard-Geschwindigkeit für die meisten Anwendungen
- **4 MHz**: Schneller, für kurze Leitungen
- **8 MHz**: Sehr schnell, nur für kurze Verbindungen empfohlen

**Auswahl-Kriterien:**
- **Kurze Verbindungen** (<1m): Bis zu 8 MHz möglich
- **Mittlere Verbindungen** (1-3m): 2-4 MHz empfohlen
- **Lange Verbindungen** (>3m): 1-2 MHz verwenden
- **Viele LEDs** (>100): Niedrigere Frequenz für Stabilität

**Troubleshooting:**
- Flackernde LEDs: Clock-Frequenz reduzieren
- Keine Funktion: Verkabelung und Frequenz prüfen
- Falsche Farben: Meist Verdrahtungsproblem, nicht Clock-bezogen

