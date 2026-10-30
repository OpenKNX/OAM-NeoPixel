### GPIO manuell konfigurieren

Der GPIO-Pin für die Daten des LED-Streifens, wenn bei „Daten Port“ die Auswahl „Manuell“ eingestellt ist.

**Konfiguration:**

- Geben Sie die GPIO-Nummer des Mikrocontrollers ein (z. B. 16 für GPIO16)
- Nur nötig, wenn der gewünschte Anschluss nicht als Port der Hardware angeboten wird
- Der Pin muss als Ausgang verwendbar sein

**Wichtige Hinweise:**

- Pins, die das Gerät für KNX (TX/RX), den Save-Pin, seine Taster (Prog, Funktion), seine Status-LEDs, den Flash-Speicher des ESP32 (z. B. klassisch GPIO 6–11, S3 26–32 und bei Octal-Speicher 33–37, C3 12–17, C6 24–26 und 28–30) oder bei Ethernet-Boards (QuinLED-Ethernet-Varianten, Gledopto GL-C-620WL) die Ethernet-Schnittstelle benutzt, lehnt die Firmware ab: der Streifen bleibt dann aus (Log: „REFUSED“)
- Nutzt ein Relais oder ein vorheriger Streifen denselben GPIO schon, lehnt die Firmware den späteren Streifen ab und er bleibt aus (Log: „REFUSED“); ein SPI-Streifen belegt MOSI und Takt nur gemeinsam.
- ESP32 (klassisch): GPIO 34–39 sind nur Eingänge
- Ohne Auswahl bei „Daten Port“ („Wählen Sie bitte ein Port aus“) bleibt der Streifen aus

**Empfehlung:**
Wenn möglich einen Port der Hardware auswählen; „Manuell“ nur für eigene Verdrahtung verwenden.
