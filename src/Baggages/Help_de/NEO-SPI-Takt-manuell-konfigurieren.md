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

**Reservierte Pins:** Ein manuell gewählter Clock-GPIO, den das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion) oder seine Status-LEDs benutzt, wird abgelehnt: der Streifen bleibt dann aus (Log: „REFUSED“).
