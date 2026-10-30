### SPI Takt manuell konfigurieren

Ermöglicht die manuelle Konfiguration der SPI-Taktfrequenz für SPI-basierte LED-Chips wie APA102, SK9822 oder WS2801.

**Standardverhalten (Aus):**

- Das System wählt automatisch eine optimale Taktfrequenz basierend auf dem LED-Typ
- Für die meisten Anwendungen ausreichend und empfohlen
- Gewährleistet maximale Kompatibilität

**Manuelle Konfiguration (Ein):**

- Ermöglicht die Auswahl einer spezifischen SPI-Taktfrequenz
- Zeigt zusätzliche Optionen für die Clock-Geschwindigkeit an
- Nützlich für spezielle Anwendungen oder Optimierungen

**Wann verwenden:**

- **Probleme mit Standardeinstellung**: Flackern oder instabile LED-Anzeige
- **Lange Kabelwege**: Niedrigere Frequenz für bessere Stabilität
- **Performance-Optimierung**: Höhere Frequenz für schnelle Updates
- **Spezielle Hardware**: Anpassung an besondere Anforderungen

**Empfehlung:**
Für die meisten Installationen sollte diese Option **deaktiviert** bleiben, da die automatische Einstellung optimal funktioniert.

**Reservierte Pins:** Ein manuell gewählter Clock-GPIO, den das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion), seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, wird abgelehnt: der Streifen bleibt dann aus (Log: „REFUSED“).
