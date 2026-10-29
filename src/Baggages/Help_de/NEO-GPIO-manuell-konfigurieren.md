### GPIO manuell konfigurieren

Ermöglicht die manuelle Auswahl des GPIO-Pins für SPI-basierte LED-Chips anstelle der automatischen Pin-Zuweisung.

**Standardverhalten (Aus):**

- Das System verwendet die standardmäßig konfigurierten GPIO-Pins
- Clock GPIO und MOSI GPIO werden automatisch zugewiesen
- Für die meisten Anwendungen ausreichend und empfohlen

**Manuelle Konfiguration (Ein):**

- Ermöglicht die freie Auswahl eines GPIO-Pins
- Zeigt zusätzliche Optionen für die Pin-Auswahl an
- Überschreibt die Standard-GPIO-Einstellungen

**Anwendungsfälle:**

- **Pin-Konflikte**: Wenn Standard-GPIOs bereits anderweitig verwendet werden
- **Hardware-Anpassungen**: Spezielle Platinen oder Adapter verwenden andere Pins
- **Optimierung**: Bessere Routing-Möglichkeiten auf der Platine
- **Debugging**: Test verschiedener GPIO-Pins bei Problemen

**Wichtige Hinweise:**

- Nur für SPI-basierte LED-Typen (APA102, WS2801, etc.) verfügbar
- Stellen Sie sicher, dass der gewählte Pin als GPIO verwendbar ist
- Vermeiden Sie GPIOs, die für andere Funktionen reserviert sind
- Pins, die das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion) oder seine Status-LEDs benutzt, lehnt die Firmware ab: der Streifen bleibt dann aus (Log: „REFUSED“)

**Empfehlung:**
Nur aktivieren wenn spezielle Hardware-Anforderungen vorliegen oder Pin-Konflikte auftreten.