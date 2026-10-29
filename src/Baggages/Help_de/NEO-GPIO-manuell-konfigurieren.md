### GPIO manuell konfigurieren

Der GPIO-Pin für die Daten des LED-Streifens, wenn bei „Daten Port“ die Auswahl „Manuell“ eingestellt ist.

**Konfiguration:**

- Geben Sie die GPIO-Nummer des Mikrocontrollers ein (z. B. 16 für GPIO16)
- Nur nötig, wenn der gewünschte Anschluss nicht als Port der Hardware angeboten wird
- Der Pin muss als Ausgang verwendbar sein

**Wichtige Hinweise:**

- Pins, die das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion) oder seine Status-LEDs benutzt, lehnt die Firmware ab: der Streifen bleibt dann aus (Log: „REFUSED“)
- ESP32 (klassisch): GPIO 6–11 sind auf den meisten Modulen mit dem Flash-Speicher verbunden und dürfen dort nicht verwendet werden; GPIO 34–39 sind nur Eingänge
- Ohne Auswahl bei „Daten Port“ („Wählen Sie bitte ein Port aus“) bleibt der Streifen aus

**Empfehlung:**
Wenn möglich einen Port der Hardware auswählen; „Manuell“ nur für eigene Verdrahtung verwenden.
