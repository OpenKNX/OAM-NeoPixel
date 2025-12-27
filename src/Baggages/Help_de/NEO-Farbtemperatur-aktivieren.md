# Farbtemperatur aktivieren

Aktiviert die HCL (Human Centric Lighting) Farbtemperatur-Steuerung für die gesamte Beleuchtung.

Wenn diese Option aktiviert ist, wird die Farbtemperatur über das HCL-System gesteuert. Dies ermöglicht:
- **Automatische Anpassung** über HCL-Kurven (Sonnenstand oder Zeitplan)
- **Manuelle Steuerung** über KNX-Objekte (2700K-6500K)
- **Post-Processing** der Effekte (Farben bleiben erhalten, nur die Farbtemperatur wird angepasst)

## Wichtig:
- Das HCL-System arbeitet als "True HCL" - es überschreibt **nicht** die aktiven Effekte oder Szenen
- Stattdessen wird die Farbtemperatur auf die bereits gerenderten Pixel angewendet
- Saturierte Farben werden weniger stark beeinflusst als weisse oder pastellfarbene Pixel

## Voraussetzungen:
- HCL-Kurve muss konfiguriert sein (Typ: Sonne oder Zeit)
- Min/Max Kelvin-Werte müssen gesetzt sein (z.B. 2700K - 6500K)
- Für optimale Ergebnisse: LED-Streifen mit RGBW für bessere Farbtemperaturwiedergabe

**Anwendung**: Ideal für circadiane Beleuchtung, die sich automatisch an die Tageszeit anpasst (warmes Licht morgens/abends, kühles Licht mittags).