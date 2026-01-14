# Farbtemperatur aktivieren

Aktiviert die HCL (Human Centric Lighting) Farbtemperatur-Steuerung für die gesamte Beleuchtung.

Wenn diese Option aktiviert ist, wird die Farbtemperatur über das HCL-System gesteuert. Dies ermöglicht:
- **Automatische Anpassung** über HCL-Kurven (Sonnenstand oder Zeitplan)
- **Manuelle Steuerung** über KNX-Objekte (2700K-6500K)
- **Post-Processing** der Effekte (Farben bleiben erhalten, nur die Farbtemperatur wird angepasst)

## Funktionsweise nach LED-Typ:

### RGBCCT (5-Kanal) Streifen - Empfohlen!
Für LED-Streifen mit separatem Warm-Weiss (WW) und Kalt-Weiss (CW) Kanal:
- Das HCL-System steuert **direkt das WW/CW-Verhältnis**
- **2700K**: WW voll, CW aus → warmes Licht
- **6500K**: WW aus, CW voll → kühles Tageslicht
- **Dazwischen**: Proportionale Mischung für reine Weißtöne
- **Vorteil**: Reine, effiziente Farbtemperaturwiedergabe ohne RGB-Mischung

### RGBW (4-Kanal) und RGB (3-Kanal) Streifen
- Das HCL-System wendet eine **RGB-Tönung** basierend auf der Kelvin-Temperatur an
- Die RGB-Kanäle werden entsprechend der Farbtemperatur eingefärbt
- Saturierte Farben werden weniger stark beeinflusst als weiße oder pastellfarbene Pixel

## Wichtig:
- Das HCL-System arbeitet als "True HCL" - es überschreibt **nicht** die aktiven Effekte oder Szenen
- Stattdessen wird die Farbtemperatur auf die bereits gerenderten Pixel angewendet
- Die Transformation erfolgt **nach** dem Effekt-Rendering, **vor** dem Senden an die Hardware

## Voraussetzungen:
- HCL-Kurve muss konfiguriert sein (Typ: Sonne oder Zeit)
- Min/Max Kelvin-Werte müssen gesetzt sein (z.B. 2700K - 6500K)
- Das Gerät benötigt eine korrekte Systemzeit (via NTP oder KNX)

**Empfehlung**: Für beste Ergebnisse verwenden Sie **RGBCCT (5-Kanal)** Streifen, die eine präzise und energieeffiziente Farbtemperatursteuerung ermöglichen.