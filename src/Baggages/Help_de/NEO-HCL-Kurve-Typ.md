# HCL-Kurve-Typ

Wählt die Art der HCL-Kurve (Human Centric Lighting) für die **automatische Farbtemperaturanpassung**.

## Optionen:

### Keine (Deaktiviert)
- HCL-Kurve ist ausgeschaltet
- Farbtemperatur wird nur manuell über KNX-Objekte gesteuert

### Sonne (Sonnenstand)
- Passt die Farbtemperatur automatisch basierend auf Sonnenaufgang/-untergang an
- **Morgens/Abends**: Warmes Licht (Minimum Kelvin, z.B. 2700K)
- **Mittags**: Kühles Tageslicht (Maximum Kelvin, z.B. 6500K)
- Verwendet Sinuskurve für sanfte Übergänge
- Benötigt GPS-Koordinaten oder NTP-Zeitabgleich

### Zeit (Zeitbasiert)
- Passt die Farbtemperatur nach einem festen Zeitplan an (z.B. 06:00-22:00)
- **Am Anfang/Ende des Zeitfensters**: Warmes Licht (Minimum Kelvin)
- **In der Mitte des Zeitfensters**: Kühles Tageslicht (Maximum Kelvin)
- Verwendet Sinuskurve für sanfte Übergänge
- Unabhängig vom Sonnenstand, funktioniert auch ohne GPS

## Funktionsweise:
Die HCL-Kurve berechnet **einmal pro Minute** den optimalen Kelvin-Wert basierend auf der aktuellen Uhrzeit und sendet ihn an das KNX-Objekt. Die Farbtemperatur wird dann als Post-Processing auf alle gerenderten Pixel angewendet.

**Wichtig**: Das System benötigt eine **korrekte Systemzeit** (via NTP oder KNX), sonst wird eine Warnung im Log ausgegeben.