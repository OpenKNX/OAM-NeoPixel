### Anzahl an LEDs/LED-Gruppen

Die Anzahl der LEDs oder LED-Gruppen in dem angeschlossenen LED-Streifen. Bei einfachen LED-Streifen entspricht dies der tatsächlichen LED-Anzahl, bei Streifen mit gruppierten LEDs (z.B. WS2815 mit 3 LEDs pro Gruppe) der Anzahl der steuerbaren Gruppen. Diese Einstellung ist kritisch für die korrekte Funktion und Stromberechnung.

**Wichtige Hinweise:**
- Geben Sie die exakte Anzahl der LEDs oder LED-Gruppen ein
- Bei gruppierten LEDs (z.B. WS2815): Anzahl der steuerbaren Gruppen, nicht der einzelnen LEDs
- Bei einfachen LEDs (z.B. WS2812B): Anzahl der einzelnen LEDs
- Zu hohe Werte können zu Flackern führen
- Zu niedrige Werte bedeuten, dass nicht alle LEDs/Gruppen angesteuert werden

**Grenzen:**
- Minimum: 1 LED/LED-Gruppe
- Maximum: Abhängig von verfügbarem Speicher und Stromversorgung
- Empfohlen: Bis zu 300 LEDs/LED-Gruppen pro Kanal für optimale Performance

**Stromberechnung:**
Die Anzahl wird für die automatische Stromgrenzbegrenzung verwendet. Bei 30 LED-Gruppen und 60mA pro Gruppe ergibt das 1,8A maximalen Strom. Beachten Sie bei gruppierten LEDs den tatsächlichen Stromverbrauch aller LEDs in einer Gruppe.

**Performance-Tipp:**
Sehr lange LED-Streifen (>500 LEDs/LED-Gruppen) können die Update-Rate verringern. Teilen Sie lange Streifen auf mehrere Kanäle auf.

